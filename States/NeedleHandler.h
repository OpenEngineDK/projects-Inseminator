#ifndef _NEEDLE_HANDLER_H_
#define _NEEDLE_HANDLER_H_

#include "Spermatozoa.h"
#include "MediPhysic.h"

#include <Core/GameEngine.h>
#include <Devices/IMouse.h>
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>
#include <Scene/ISceneNode.h>
#include <Renderers/OpenGL/TextureLoader.h>
#include <Logging/Logger.h>

using OpenEngine::Core::GameEngine;
using OpenEngine::Scene::ISceneNode;
using OpenEngine::Renderers::OpenGL::TextureLoader;

using namespace OpenEngine::Logging;
using namespace OpenEngine::Resources;

static const float speed = 0.05f;
static const float MAX_TIME = 120000.0; // in millisecs
static const float MAX_MOUSE_SPEED = 0.33;

class NeedleHandler : public IModule {
private:
    float timer;

    TransformationNode* needle;
    Spermatozoa* spermatozoa;
    IMouse* mus;
    ISceneNode* rootNode;

    Listener<NeedleHandler, KeyboardEventArg>* keyDown;
    Listener<NeedleHandler, KeyboardEventArg>* keyUp;

    bool sucking, suckedUpRight, suckEnabled;
    int lx, ly;                      // last mouse position
    bool init;
    bool left, right;
    int dx, dy;
    MouseState s;

    Spermatozoa* released;

public:
 NeedleHandler(MediPhysic* mediPhysic, ISceneNode* root) 
        : needle(NULL), spermatozoa(NULL), mus(NULL), rootNode(root), 
          lx(0), ly(0), init(true), left(false), right(false),  released(NULL) {

        timer = 0.0f;
        suckEnabled = false;

        // Keyboard bindings
        keyDown = new Listener<NeedleHandler, KeyboardEventArg> (*this, &NeedleHandler::KeyDown);
        keyUp   = new Listener<NeedleHandler, KeyboardEventArg> (*this, &NeedleHandler::KeyUp);

        // Load Needle model
        IModelResourcePtr mod = ResourceManager::CreateModel("Needle.obj");
        mod->Load();
        if( mod->GetFaceSet() == NULL )
            logger.error << "Loading needle obj file failed - FaceSet is empty!" << logger.end;
        GeometryNode* gNode = new GeometryNode();
        gNode->SetFaceSet(mod->GetFaceSet());

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
        suckEnabled = false;
        init = true;
        mus = dynamic_cast<IMouse*>(IGameEngine::Instance().Lookup(typeid(IMouse)));
        mus->HideCursor();

        // Keyboard bindings
        IKeyboard::keyDownEvent.Add(keyDown);
        IKeyboard::keyUpEvent.Add(keyUp);

        needle->SetPosition(Vector<3,float>(5,1,0.0));
        rootNode->AddNode(needle);

        if( spermatozoa != NULL ){
            // add spermatozoa from scene graph
            //logger.info << "spermatozoa added in needlehander" << logger.end;
            released = NULL;
        }

        TextureLoader texLoad;
        rootNode->Accept(texLoad);
    }

    void Deinitialize() {
        if( spermatozoa != NULL ){
            // remove spermatozoa from scene graph
            rootNode->RemoveNode(spermatozoa->GetTransformation());
        }
        rootNode->RemoveNode(needle);
        IKeyboard::keyDownEvent.Remove(keyDown);
        IKeyboard::keyUpEvent.Remove(keyUp);
    }

    bool IsTypeOf(const std::type_info& inf) { return false; }
    void Process(const float deltaTime, const float percent) {
        timer += deltaTime;
        if (timer > MAX_TIME) {
            timer = 0;
            logger.info << "timout off " << MAX_TIME << " mSec, simulator was restarted" << logger.end;
            IGameEngine::Instance().Stop();
        }

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
        //const float minX = -6, maxX = 12, minY = -5.5, maxY = 5.5;
        const float minX = -2, maxX = 8, minY = -6.5, maxY = 6.5;

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
            needle->AddNode(spermatozoa->GetTransformation());
        }
        needle->SetPosition(Vector<3,float>(5, 1, 0.0));
        released = NULL;
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
                timer = 0.0f;
			}
            if (right && suckEnabled) {
				Vector<3,float> pos = spermatozoa->GetPosition();
                pos[0] -= speed;
				spermatozoa->SetPosition(pos);
                timer = 0.0f;
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
        // on the first process we need to set the mouse and use the set
        // values as the current mouse state. If not we give rise to a
        // race condition where the mouse state will depend on the order
        // that this module and the IMouse module have been added to the
        // engine.
        if (init) {
            init = false;
            mus->SetCursor(400,300);
            s.x = lx = 400;
            s.y = ly = 300;
            s.buttons = BUTTON_NONE; //@todo: not initialized
        } else {
            s = mus->GetState();
        }

        // compute rotate difference
        dx = lx - s.x; // moving mouse up = positive value
        dy = ly - s.y; // moving to the left = positive value

        if (dx != 0 || dy != 0)
            timer = 0; // renew timer

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
