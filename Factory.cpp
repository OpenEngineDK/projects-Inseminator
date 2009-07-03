//------------------- Settings ----------------------------
//static const int FRAME_WIDTH = 1680, FRAME_HEIGHT = 1050;
//static const int FRAME_WIDTH = 1440, FRAME_HEIGHT = 900;
static const int FRAME_WIDTH = 770, FRAME_HEIGHT = 450;

static const bool FULLSCREEN = false;
//static const bool FULLSCREEN = true;

static const int RESET_TIME = 20000; // in milli seconds
//---------------------------------------------------------

#include "Factory.h"

#include "PictureMovieResource.h"
//#include "InitGLNode.h"
#include "TimedQuitEventHandler.h"
#include "States/HUDisplay.h"
#include "States/Background.h"

#include "States/HitTheLittleGuyState.h"
#include "States/TurnTheEggState.h"
#include "States/InseminateState.h"
#include "States/SelectState.h"

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
#include <Resources/SDLImage.h>

#include <Scene/PointLightNode.h>
#include <Scene/BlendingNode.h>
#include <Scene/SceneNode.h>
#include <Scene/RenderStateNode.h>

// from extensions
#include <Resources/IMovieResource.h>
#include <Resources/FFMPEGResource.h>
#include <Utils/QuitHandler.h>
#include <Utils/Billboard.h>
#include <MediPhysic.h>

#include <Utils/MoveHandler.h>
#include <Display/HUD.h>

#include <Meta/OpenGL.h>
#include <Meta/SDL.h>

using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

MovieState* Factory::CreatePMState(std::string file, std::string nextState, 
                                   float duration) {
        ITextureResourcePtr donateTextStateTexture = 
            ResourceManager<ITextureResource>::Create(file);
        donateTextStateTexture->Load();
        IMovieResourcePtr donateTextStateMovie = IMovieResourcePtr
            (new PictureMovieResource(donateTextStateTexture ,duration));
        //IMovieResourcePtr donateTextStateMovie = 
        //  ResourceManager<IMovieResource>::Create("DonateText.mov");
        MovieState* donateTextState = new MovieState
            (donateTextStateMovie, nextState, false, *so);
        return donateTextState;
}

MovieState* Factory::CreateMState(std::string file, std::string nextState, 
                                   bool continueToNextState) {
        IMovieResourcePtr introStateMovie = 
            ResourceManager<IMovieResource>::Create(file);
        MovieState* introState = new MovieState
            (introStateMovie, nextState, true, *so, continueToNextState);
        return introState;
}

bool Factory::SetupEngine(IEngine& engine, std::string startState) {
    try {
        engine.InitializeEvent().Attach(*frame);
        engine.ProcessEvent().Attach(*frame);
        engine.DeinitializeEvent().Attach(*frame);

        /*
        // Hack to avoid white flash at startup
        ((SDLFrame*)frame)->Handle(InitializeEventArg());
        Vector<4,float> bgc(0.0, 0.0, 0.0, 1.0);
        glClearColor(bgc[0], bgc[1], bgc[2], bgc[3]);
        SDL_GL_SwapBuffers();
        */

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
        string billboardsdir = resourcedir + "billboards/";
        DirectoryManager::AppendPath(billboardsdir);
        string moviesdir = resourcedir + "movies/";
        DirectoryManager::AppendPath(moviesdir);
        string picturemoviesdir = resourcedir + "picturemovies/";
        DirectoryManager::AppendPath(picturemoviesdir);
        
        // load the resource plug-ins
        ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());
        ResourceManager<ITextureResource>::AddPlugin(new SDLImagePlugin());
        ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());

        HUD* hud = new HUD(FRAME_WIDTH, FRAME_HEIGHT);
        renderer->PostProcessEvent().Attach(*hud);

        RenderStateNode* rsn = new RenderStateNode();
        rsn->DisableOption(RenderStateNode::SHADER);
        rsn->EnableOption(RenderStateNode::LIGHTING);
        renderer->SetSceneRoot(rsn);

        // Create scene root
        BlendingNode* root = new BlendingNode();
        rsn->AddNode(root);
        
        StateManager* sm = new StateManager();
        so = new StateObjects(root, sm, mouse, *keyboard, engine, *tl, *hud);

        // Create MediPhysic module handling the sphere (Eeg)
        MediPhysic* physic = new MediPhysic(modeldir,*tl);
        // MediPhysics needs the full path to resources because
        // it does not use the ResourceManager to load models
        
        // Create needle handler
        NeedleHandler* needleHandler = 
            new NeedleHandler(physic, root, mouse, *keyboard, engine, *tl);
        physic->needle = needleHandler->GetTransformationNode();

        // Add noisy floating textures to the background
        Background* bg = new Background("Background.png", root);

        // 01. Start up picture
        PictureState* startup = new PictureState
            ("Intro.png", "IntroState", *so);

        // 02. Intro (Video)
        MovieState* introState = 
            CreateMState("Intro.mov", "DonateTextState");

        // 03. "Morten has Donated" (Video)
        MovieState* donateTextState =
            //CreateMState("DonateText.mov", "DonateState");
            CreatePMState("DonateText.png", "DonateState", 9800);

        // 04.
        MovieState* donateState = 
            //CreateMState("Donate.mov", "HitTheLittleGuyText1");
            CreatePMState("Donate.png", "HitTheLittleGuyText1", 7990);

        // 05.
        MovieState* hitText1 =
            //CreateMState("HitTheLittleGuyText1.mov", "HitTheLittleGuyText2");
            CreatePMState("HitTheLittleGuyText1.png", "HitTheLittleGuyText2",9880);
        
        // 06.
        MovieState* hitText2 = 
            //CreateMState("HitTheLittleGuyText2.mov", "HitTheLittleGuyState");
            CreatePMState("HitTheLittleGuyText2.png", "HitTheLittleGuyState",9840);

        // 07. "Hit The Little Guy" (Simulator)
        list<Spermatozoa*>* spermList;
        spermList = needleHandler->InitializeSpermatazoaList(*so);

        HitTheLittleGuyState* hitState = new HitTheLittleGuyState
            ("HitTheLittleGuySuccessState", *so);
        hitState->SetNeedle(needleHandler);
        hitState->SetBackground(bg);
        hitState->SetSpermatozoaList(spermList);
        
        // 08. Successfully accomplished (Video) 
        MovieState* hitSuccessState = 
            //CreateMState("HitTheLittleGuySuccess.mov", "SelectTheLittleGuyText");
            CreatePMState("HitTheLittleGuySuccess.png", "SelectTheLittleGuyText",3920);
        
        // 09.
        MovieState* selectText = 
            //CreateMState("SelectTheLittleGuyText.mov", "SelectTheLittleGuyState");
            CreatePMState("SelectTheLittleGuyText.png", "SelectTheLittleGuyState", 8000);

        // 10. "Select The Little Guy" (Simulator)
        SelectState* selectState = new SelectState
            ("SelectTheLittleGuySuccessState", *so);
        selectState->SetNeedle(needleHandler);
        selectState->SetBackground(bg);
        selectState->SetSpermatozoaList(spermList);
        
        // 11. Successfully accomplished (Video)
        MovieState* selectSuccessState = 
            //CreateMState("SelectTheLittleGuySuccess.mov", "TurnTheEggText");
            CreatePMState("SelectTheLittleGuySuccess.png", "TurnTheEggText", 9960);
        
        // 12.
        MovieState* turnText = 
            //CreateMState("TurnTheEggText.mov", "TurnTheEggState");
            CreatePMState("TurnTheEggText.png", "TurnTheEggState", 6680);
        
        // 13. "Turn The Egg" (Simulator)
        TurnTheEggState* turnState = new TurnTheEggState
            ("TurnTheEggSuccessState", physic, engine, *so);
        turnState->SetNeedle(needleHandler);
        turnState->SetBackground(bg);
        
        // 14. Successfully accomplished (Video) 
        MovieState* turnSuccessState = 
            //CreateMState("TurnTheEggSuccess.mov", "InseminationText");
            CreatePMState("TurnTheEggSuccess.png", "InseminationText", 10080);
        
        // 15. Insemination (Simulator)
        MovieState* inseminateText = 
            //CreateMState("InseminationText.mov", "InseminationState");
            CreatePMState("InseminationText.png", "InseminationState", 10000);
        
        // 16. "Insemination" (Simulator)
        InseminateState* inseminateState = new InseminateState
            ("InseminationSuccessState", physic, *so);
        inseminateState->SetNeedle(needleHandler);
        inseminateState->SetBackground(bg);

        // 17. Successfully accomplished (Video)
        MovieState* inseminateSuccessState = 
            //CreateMState("InseminationSuccess.mov", "Outro");
            CreatePMState("InseminationSuccess.png", "Outro", 15800);

        // 18. Outro (Video)
        MovieState* outro = 
            CreateMState("Outro.mov", "StartupPicture", true);


        // Create and initialize StateManager
        sm->AddState("StartupPicture", startup);

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

        sm->SetInitialState(startState);

        engine.InitializeEvent().Attach(*sm);
        engine.ProcessEvent().Attach(*sm);
        engine.DeinitializeEvent().Attach(*sm);

        // global quit event handlers
        TimedQuitEventHandler* tquit_h = 
            new TimedQuitEventHandler(RESET_TIME,engine);
        keyboard->KeyEvent().Attach(*tquit_h);

        QuitHandler* quit_h = new QuitHandler(engine);
        keyboard->KeyEvent().Attach(*quit_h);

        /* for demo mode
        MoveHandler* move_h = 
            new MoveHandler(*camera, *mouse);
        move_h->SetObjectMove(false);
        keyboard->KeyEvent().Attach(*move_h);
        engine.InitializeEvent().Attach(*move_h);
        engine.ProcessEvent().Attach(*move_h);
        engine.DeinitializeEvent().Attach(*move_h);
        */

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
