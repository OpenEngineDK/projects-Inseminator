//------------------- Settings ----------------------------
//static const int FRAME_WIDTH = 1680, FRAME_HEIGHT = 1050;
//static const int FRAME_WIDTH = 1440, FRAME_HEIGHT = 900;
static const int FRAME_WIDTH = 770, FRAME_HEIGHT = 450;

static const bool FULLSCREEN = false;
//static const bool FULLSCREEN = true;

static const int RESET_TIME = 20000; // in milli seconds
//---------------------------------------------------------

#include "Factory.h"

//#include "InitGLNode.h"
#include "TimedQuitEventHandler.h"
#include "States/StateObjects.h"
#include "States/HUDisplay.h"
#include "States/Background.h"

#include "States/HitTheLittleGuyState.h"
#include "States/TurnTheEggState.h"
#include "States/InseminateState.h"
#include "States/SelectState.h"

#include "States/MovieState.h"
#include "States/PictureState.h"
#include "States/NeedleHandler.h"

#include <Display/Frustum.h>
#include <Display/ViewingVolume.h>
#include <Display/SDLFrame.h>
#include <Display/Camera.h>
#include <Devices/SDLInput.h>

#include <Renderers/OpenGL/LightRenderer.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>
#include <Resources/OBJResource.h>
#include <Renderers/TextureLoader.h>
#include <Resources/TGAResource.h>

#include <Scene/PointLightNode.h>
#include <Scene/BlendingNode.h>

// from extensions
#include <Resources/IMovieResource.h>
#include <Resources/FFMPEGResource.h>
#include <Utils/QuitHandler.h>
#include <Utils/Billboard.h>
#include <MediPhysic.h>

using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

bool Factory::SetupEngine(IEngine& engine) {
    try {
        engine.InitializeEvent().Attach(*frame);
        engine.ProcessEvent().Attach(*frame);
        engine.DeinitializeEvent().Attach(*frame);

        // setup renderer
        engine.InitializeEvent().Attach(*renderer);
        engine.ProcessEvent().Attach(*renderer);
        engine.DeinitializeEvent().Attach(*renderer);
        TextureLoader* tl = new TextureLoader(*renderer);
        renderer->InitializeEvent().Attach(*tl);
        renderer->PreProcessEvent().Attach(*tl);

        // Setup input handling
        SDLInput* input = new SDLInput();
        IKeyboard* keyboard = input;
        IMouse* mouse = input;
        engine.InitializeEvent().Attach(*input);
        engine.ProcessEvent().Attach(*input);
        engine.DeinitializeEvent().Attach(*input);

        string resourcedir = "./projects/Inseminator/data/";
        logger.info << "Resource directory: " << resourcedir << logger.end;
        string modeldir = resourcedir + "models/";
        DirectoryManager::AppendPath(modeldir);
        string videodir = resourcedir + "videos/";
        DirectoryManager::AppendPath(videodir);
        
        // load the resource plug-ins
        ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());
        ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());
        ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());

        // Create scene root
        BlendingNode* root = new BlendingNode();
        root->SetSource(BlendingNode::SRC_COLOR);
        root->SetDestination(BlendingNode::ONE);
        root->SetEquation(BlendingNode::REVERSE_SUBTRACT);
        renderer->SetSceneRoot(root);

        // Light settings.
        PointLightNode* light0 = new PointLightNode();
        light0->ambient = Vector<4,float>(1.0, 1.0, 1.0, 1.0);
        TransformationNode* light0Position = new TransformationNode();
        light0Position->SetPosition(Vector<3,float>(0.0, 0.0, 10.0));
        light0Position->AddNode(light0);
        root->AddNode(light0Position);

        StateManager* sm = new StateManager();
        StateObjects* so = 
            new StateObjects(root, sm, mouse, *keyboard, engine, *tl);

        // Create MediPhysic module handling the sphere (Eeg)
        MediPhysic* physic = new MediPhysic(modeldir,*tl);
        // MediPhysics needs the full path to resources because
        // it does not use the ResourceManager to load models
        
        // Create needle handler
        NeedleHandler* needleHandler = 
	  new NeedleHandler(physic, root, mouse, *keyboard, engine, *tl);
        physic->needle = needleHandler->GetTransformationNode();

        // Add noisy floating textures to the background
        Background* bg = new Background("Background.tga",root);

        // 01. Start up picture
        PictureState* startup = new PictureState
            ("Intro.tga", "IntroState", *so);
        
        // 02. Intro (Video)
        MovieState* introState = new MovieState
            ("Intro.mov", "DonateTextState", *so);
        
        // 03. "Morten has Donated" (Video)
        MovieState* donateTextState = new MovieState
            ("DonateText.mov", "DonateState", *so);
        
        // 04.
        MovieState* donateState = new MovieState
            ("Donate.mov", "HitTheLittleGuyText1", *so);
        
        // 05.
        MovieState* hitText1 = new MovieState
            ("HitTheLittleGuyText1.mov", "HitTheLittleGuyText2", *so);
        
        // 06.
        MovieState* hitText2 = new MovieState
            ("HitTheLittleGuyText2.mov", "HitTheLittleGuyState", *so);

        // 07. "Hit The Little Guy" (Simulator)
        HitTheLittleGuyState* hitState = new HitTheLittleGuyState
            ("HitTheLittleGuySuccessState", *so);
        hitState->SetNeedle(needleHandler);
        hitState->SetBackground(bg);
        
        // 08. Successfully accomplished (Video) 
        MovieState* hitSuccessState = new MovieState
            ("HitTheLittleGuySuccess.mov", "SelectTheLittleGuyText", *so);
        
        // 09.
        MovieState* selectText = new MovieState
            ("SelectTheLittleGuyText.mov", "SelectTheLittleGuyState", *so);

        // 10. "Select The Little Guy" (Simulator)
        SelectState* selectState = new SelectState
            ("SelectTheLittleGuySuccessState", *so);
        selectState->SetNeedle(needleHandler);
        selectState->SetBackground(bg);
        selectState->SetSpermatozoaList(hitState->GetSpermatozoaList());
        
        // 11. Successfully accomplished (Video)
        MovieState* selectSuccessState = new MovieState
            ("SelectTheLittleGuySuccess.mov", "TurnTheEggText", *so);
        
        // 12.
        MovieState* turnText = new MovieState
            ("TurnTheEggText.mov", "TurnTheEggState", *so);
        
        // 13. "Turn The Egg" (Simulator)
        TurnTheEggState* turnState = new TurnTheEggState
            ("TurnTheEggSuccessState", physic, engine, *so);
        turnState->SetNeedle(needleHandler);
        turnState->SetBackground(bg);
        
        // 14. Successfully accomplished (Video) 
        MovieState* turnSuccessState = new MovieState
            ("TurnTheEggSuccess.mov", "InseminationText", *so);
        
        // 15. Insemination (Simulator)
        MovieState* inseminateText = new MovieState
            ("InseminationText.mov", "InseminationState", *so);
        
        // 16. "Insemination" (Simulator)
        InseminateState* inseminateState = new InseminateState
            ("InseminationSuccessState", physic, *so);
        inseminateState->SetNeedle(needleHandler);
        inseminateState->SetBackground(bg);

        // 17. Successfully accomplished (Video)
        MovieState* inseminateSuccessState = new MovieState
            ("InseminationSuccess.mov", "Outro", *so);

        // 18. Outro (Video)
        MovieState* outro = new MovieState
            ("Outro.mov", "StartupPicture", *so, true);


        // Create and initialize StateManager
        sm->AddStateAsInitial("StartupPicture", startup);

        sm->AddState("IntroState", introState);
        sm->AddState("DonateTextState", donateTextState);
        sm->AddState("DonateState", donateState);

        sm->AddState("HitTheLittleGuyText1", hitText1);
        sm->AddState("HitTheLittleGuyText2", hitText2);
        sm->AddState("HitTheLittleGuyState", hitState);
        sm->AddState("HitTheLittleGuySuccessState", hitSuccessState);

        sm->AddState("SelectTheLittleGuyText", selectText);
        sm->AddState("SelectTheLittleGuyState", selectState);
        sm->AddState("SelectTheLittleGuySuccessState", selectSuccessState);

        sm->AddState("TurnTheEggText", turnText);
        sm->AddState("TurnTheEggState", turnState);
        sm->AddState("TurnTheEggSuccessState", turnSuccessState);

        sm->AddState("InseminationText", inseminateText);
        sm->AddState("InseminationState", inseminateState);
        sm->AddState("InseminationSuccessState", inseminateSuccessState);
        sm->AddState("Outro", outro);

        engine.InitializeEvent().Attach(*sm);
        engine.ProcessEvent().Attach(*sm);
        engine.DeinitializeEvent().Attach(*sm);

        // global quit event handlers
        TimedQuitEventHandler* tquit_h = 
            new TimedQuitEventHandler(RESET_TIME,engine);
        keyboard->KeyEvent().Attach(*tquit_h);

        QuitHandler* quit_h = new QuitHandler(engine);
        keyboard->KeyEvent().Attach(*quit_h);

    } catch (const Exception& ex) {
        logger.error << "An exception occurred: " << ex.what() << logger.end;
        throw ex;
    }
    // Return true to signal success.
    return true;
}

Factory::Factory() {
    FrameOption options = FrameOption();
    if(FULLSCREEN)
      options = FRAME_FULLSCREEN;

    frame    = new SDLFrame(FRAME_WIDTH, FRAME_HEIGHT, 32, options);
    viewport = new Viewport(*frame);
        
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(3,0,-15));
    camera->LookAt(Vector<3,float>(3,0,0));
    //viewport->SetViewingVolume(camera);

    // frustum hack
    Frustum* frustum = new Frustum(*camera);
    //frustum->SetFar(1000);
    viewport->SetViewingVolume(frustum);
      
    renderer = new Renderer(viewport);
    // Add a rendering view to the renderer
    renderer->ProcessEvent().Attach(*(new RenderingView(*viewport)));
    renderer->PreProcessEvent().Attach(*(new LightRenderer(*camera)));
}

Factory::~Factory() {
  delete frame;
  delete renderer;
  delete viewport;
  delete camera;
}

IFrame* Factory::GetFrame() { return frame; }

IRenderer* Factory::GetRenderer() { return renderer; }
