#ifndef _NEEDLE_HANDLER_H_
#define _NEEDLE_HANDLER_H_

#include "Spermatozoa.h"
#include "MediPhysic.h"
#include "StateObjects.h"

#include <Core/IEngine.h>
#include <Core/IState.h>
#include <Devices/IMouse.h>
#include <Devices/IKeyboard.h>
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>
#include <Scene/ISceneNode.h>
#include <Renderers/TextureLoader.h>
#include <Logging/Logger.h>


using OpenEngine::Scene::ISceneNode;
using OpenEngine::Renderers::TextureLoader;

using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Devices;

static const float speed = 0.05f;
static const float MAX_MOUSE_SPEED = 0.36; // former 0.33

// initialize spermatazoa list
const float SPERM_SPEED = -0.0007f;
const int NUM_SPERMATOZOA = 6;
const int SEED = 0;

class NeedleHandler : public IState, public IListener<KeyboardEventArg> {
private:
    TransformationNode* needle;
    Spermatozoa* spermatozoa;
    IMouse* mus;
    IKeyboard& keyboard;
    IEngine& engine;
    ISceneNode* rootNode;
    TextureLoader& textureLoader;

    bool sucking, suckedUpRight, suckEnabled;
    int lx, ly;                      // last mouse position
    bool init;
    bool left, right;
    int dx, dy;
    MouseState s;

    Spermatozoa* released;

public:
 NeedleHandler(MediPhysic* mediPhysic, ISceneNode* root, IMouse* mouse,
               IKeyboard& keyboard, IEngine& engine,
               TextureLoader& textureLoader) 
   : needle(NULL), spermatozoa(NULL), mus(mouse), keyboard(keyboard),
        engine(engine), rootNode(root), textureLoader(textureLoader), 
        lx(0), ly(0), init(true), left(false), right(false),  released(NULL) {

        suckEnabled = false;

        // Load Needle model
        IModelResourcePtr mod = 
            ResourceManager<IModelResource>::Create("Needle.obj");
        mod->Load();
        if (mod->GetSceneNode() == NULL)
            logger.error << "Loading needle obj file failed" << logger.end;
        ISceneNode* gNode = mod->GetSceneNode();
        mod->Unload();

        needle = new TransformationNode();
        needle->AddNode(gNode);
    }

    ~NeedleHandler() {}

    void SetSpermatozoa(Spermatozoa* s){
        if( spermatozoa != NULL )
            needle->RemoveNode(spermatozoa->GetTransformation());
        spermatozoa = s;
        s->SetPosition(Vector<3,float>(0,0,0));
        needle->AddNode(s->GetTransformation());
    }
    Spermatozoa* GetSpermatozoa() { return spermatozoa; }

    Spermatozoa* GetReleasedSpermatozoa() { return released; }

    void Suck(Spermatozoa* s){
        // only one spermatozoa can be sucked up.
        if( spermatozoa != NULL ) return;

        Vector<3,float> dir = needle->GetPosition() - s->GetPosition();
        dir[2] = 0; // Clear depth
        Vector<3,float> newPos = s->GetPosition() + (dir * 0.01f);

        // Calculate rotation
        Vector<3,float> horizontalPos(1.0,0,0);
        Vector<3,float> horizontalNeg(-1.0,0,0);

        Vector<3,float> sDirection = s->GetTransformation()->GetRotation().RotateVector(Vector<3,float>(1,0,0));
        float dot1 = sDirection * horizontalPos;
        float dot2 = sDirection * horizontalNeg;

        float tmpAngle1 = acos( dot1 ) * (PI/180);
        float tmpAngle2 = acos( dot2 ) * (PI/180);
        float angle = tmpAngle1 < tmpAngle2 ? tmpAngle1 : tmpAngle2;
        
        s->GetTransformation()->Rotate(0,0,angle*0.1);
        s->SetPosition(newPos);

        if( angle < 0.002f && dir.GetLength() <= 0.2f ){
            suckedUpRight = angle == tmpAngle2 ? false : true;

            if( s->IsAlive() ) s->Kill();
            rootNode->RemoveNode(s->GetTransformation());
            SetSpermatozoa(s);
        }
    }

    bool SuckedUpRight() { return suckedUpRight; }
    bool IsSucking() { return (left && suckEnabled); }
    void EnableSuck(bool b) {suckEnabled = b;}

    void Initialize() {
        mus->SetCursor(400, 300);
        s = mus->GetState();
        lx = s.x;
        ly = s.y;

        suckEnabled = false;
        init = true;

        // Keyboard bindings
        keyboard.KeyEvent().Attach(*this);

        needle->SetPosition(Vector<3,float>(5,1,0.0));
        rootNode->AddNode(needle);

        if( spermatozoa != NULL ){
            // add spermatozoa from scene graph
            //logger.info << "spermatozoa added in needlehander" << logger.end;
            released = NULL;
        }

        textureLoader.Load(*rootNode);
    }

    void Deinitialize() {
        if( spermatozoa != NULL ){
            // remove spermatozoa from scene graph
            rootNode->RemoveNode(spermatozoa->GetTransformation());
        }
        rootNode->RemoveNode(needle);
        keyboard.KeyEvent().Detach(*this);
    }

    void Process(ProcessEventArg arg) {
        // Handle events
        HandleMouse();
        HandleKeys();

        // set position of needle
        float multiplier = 0.05;
        const bool useLog = false;
        if (useLog) {
            multiplier = 0.002;
            if (dx > 0)
                needle->Move(log((float)dx)*multiplier, 0, 0);
            else if (dx < 0)
                needle->Move(-log((float)-dx)*multiplier, 0, 0);
            if (dy > 0)
                needle->Move(0, log((float)dy)*multiplier, 0);
            else if (dy < 0)
                needle->Move(0, -log((float)-dy)*multiplier, 0);
        } else {
            float moveX = dx*multiplier;
            float moveY = dy*multiplier;
            if( moveX > MAX_MOUSE_SPEED ) moveX = MAX_MOUSE_SPEED;
            if( moveY > MAX_MOUSE_SPEED ) moveY = MAX_MOUSE_SPEED;
            if( moveX < -MAX_MOUSE_SPEED ) moveX = -MAX_MOUSE_SPEED;
            if( moveY < -MAX_MOUSE_SPEED ) moveY = -MAX_MOUSE_SPEED;
            needle->Move(moveX, moveY, 0);
        }

        // bounderies on x- and y-axis for the needle
        //first: const float minX = -6, maxX = 12, minY = -5.5, maxY = 5.5;
        //second: const float minX = -2, maxX = 8, minY = -6.5, maxY = 6.5;
        const float minX = -2, maxX = 8, minY = -5.0, maxY = 5.0;

        // satisfy bounderies on x-axis for the needle
        Vector<3,float> position = needle->GetPosition();
        if (position[0] < minX)
            needle->SetPosition(Vector<3,float>(minX,position[1],position[2]));
        else if (position[0] > maxX)
            needle->SetPosition(Vector<3,float>(maxX,position[1],position[2]));

        // satisfy bounderies on y-axis for the needle
        position = needle->GetPosition();
        if (position[1] < minY)
            needle->SetPosition(Vector<3,float>(position[0],minY,position[2]));
        else if (position[1] > maxY)
            needle->SetPosition(Vector<3,float>(position[0],maxY,position[2]));
    }

    void Reset(){
        if( spermatozoa != NULL && released ){
            spermatozoa->SetPosition(Vector<3,float>(2.5, 0, 0));
	    rootNode->RemoveNode(spermatozoa->GetTransformation());
            needle->AddNode(spermatozoa->GetTransformation());
        }
        needle->SetPosition(Vector<3,float>(5, 1, 0.0));
        released = NULL;
    }

    list<Spermatozoa*>* InitializeSpermatazoaList(StateObjects& so) {
        list<Spermatozoa*>* spermList;
        spermList = new list<Spermatozoa*>();
        srand((unsigned)SEED);
        // Create list of spermatozoa's
        spermList->clear();
        for( int i=0; i<NUM_SPERMATOZOA; i++ )
            for( int j=0; j<NUM_SPERMATOZOA; j++ ){
                Spermatozoa* littleGuy = new Spermatozoa(so,true);
                float random = (rand()/(float)RAND_MAX)*10;
                float speed = SPERM_SPEED + (SPERM_SPEED/(float)10 * (random));
                littleGuy->SetSpeed(speed);
                littleGuy->LoadTexture("SpermatozoaNormal-withalpha.png");
                // Generate random numbers between 0 and 1 with 2 digits
                float randX = (rand()/(float)RAND_MAX);
                float randY = (rand()/(float)RAND_MAX);
                littleGuy->GetTransformation()->SetRotation(Quaternion<float>(Vector<3,float>(randX-0.5,randY-0.5,0).GetNormalize()));
                littleGuy->SetPosition(Vector<3,float>(4-j+randX, 3-i+randY,-0.1));
                spermList->push_back(littleGuy);
            }
        return spermList;
    }

    void HandleKeys(){
        // handle the movment of the spermatozoa
		if (spermatozoa != NULL && spermatozoa->GetPosition()[0] > -0.5) { 
            //logger.info << "X = " << spermatozoa->GetPosition()[0] << logger.end;

            if (left && suckEnabled){
				Vector<3,float> pos = spermatozoa->GetPosition();
                if (pos[0] < 4.50f) // max to the left
				pos[0] += speed;
				spermatozoa->SetPosition(pos);
			}
            if (right && suckEnabled) {
				Vector<3,float> pos = spermatozoa->GetPosition();
                pos[0] -= speed;
				spermatozoa->SetPosition(pos);
			}

            // move spermaozoa billboard
            static int count = 0;
            static float offSet = 0;
            Vector<3,float> oldPos = spermatozoa->GetPosition();
            Vector<3,float> orginalPos = oldPos - offSet;
            offSet = sin( (((float)count) /360.0)*PI)/2.0;
            spermatozoa->SetPosition(Vector<3,float>(orginalPos[0]+offSet,oldPos[1],oldPos[2]));
            count++;
            count %= 360;

		}
		else {
            // Try to inject the spermatozoa into the egg.
			if (spermatozoa != NULL) {
				needle->RemoveNode(spermatozoa->GetTransformation());
				Vector<3,float> pos = spermatozoa->GetPosition() + needle->GetPosition();
				spermatozoa->SetPosition(pos);
                released = spermatozoa;
                rootNode->AddNode(spermatozoa->GetTransformation());
                spermatozoa = NULL;
			}
		}
    }

    void HandleMouse(){
        s = mus->GetState();

        // compute rotate difference
        dx = lx - s.x; // moving mouse up = positive value
        dy = ly - s.y; // moving to the left = positive value

        // reset the position
        if (abs(s.x - 400) > 100 || abs(s.y - 300) > 100) {
            mus->SetCursor(400,300);
            lx = 400;
            ly = 300;
        } else {
            lx = s.x;
            ly = s.y;
        }
    }

    void Handle(KeyboardEventArg arg) {
      if (arg.type == EVENT_PRESS)
          KeyDown(arg);
      else
          KeyUp(arg);
    }

    void KeyDown(KeyboardEventArg arg) {
        switch ( arg.sym ) {
            case KEY_LEFT:  left  = true; break;
            case KEY_RIGHT: right = true; break;
            default: break;
        }
    }

    void KeyUp(KeyboardEventArg arg) {
        switch ( arg.sym ) {
            case KEY_LEFT:  left  = false; break;
            case KEY_RIGHT: right = false; break;
            default: break;
        }
    }

    Vector<3,float> GetPosition(){
        return needle->GetPosition();
    }

    TransformationNode* GetTransformationNode(){
        return needle;
    }
};

#endif
