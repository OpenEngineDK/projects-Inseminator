#include "Factory.h"

#include "InitGLNode.h"
#include "TimedQuitEventHandler.h"
#include "States/HUDisplay.h"
#include "States/Background.h"
#include "States/HitTheLittleGuyState.h"
#include "States/TurnTheEggState.h"
#include "States/InseminateState.h"
#include "States/SelectState.h"
#include "States/MovieState.h"
#include "States/PictureState.h"
#include "States/NeedleHandler.h"

#include <Core/IGameFactory.h>
#include <Display/ViewingVolume.h>
#include <Display/SDLFrame.h>
#include <Display/Camera.h>
#include <Devices/SDLInput.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Utils/Convert.h>

// from extensions
#include <Resources/IMovieResource.h>
#include <Resources/FFMPEGResource.h>
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>
#include <Utils/Billboard.h>
#include <GLScreenshot.h>
#include <MediPhysic.h>

using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

bool Factory::SetupEngine(IGameEngine& engine) {
    try {
        // Setup input handling
        SDLInput* input = new SDLInput();
        engine.AddModule(*input);

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
        ISceneNode* root = new InitGLNode();
        this->renderer->SetSceneRoot(root);


        // Create MediPhysic module handling the sphere (Eeg)
        MediPhysic* physic = new MediPhysic(modeldir);
	/* MediPhysics needs the full path to resources because
	   it does not use the ResourceManager to load models and textures */


        // Create needle handler
        NeedleHandler* needleHandler = new NeedleHandler(physic, root);
        physic->needle = needleHandler->GetTransformationNode();
        // Add noisy floating textures to the background
        Background* bg = new Background("Background.tga");

        // 0. Start up picture
	PictureState* startup = new PictureState
	  ("Intro.tga", "IntroState");
        // 1. Intro (Video)
        MovieState* introState = new MovieState
	  ("Intro.mov", "DonateTextState");

        // 2. "Morten has Donated" (Video)
        MovieState* donateTextState = new MovieState
	  ("DonateText.mov", "DonateState");

        // 3.
        MovieState* donateState = new MovieState
	  ("Donate.mov", "HitTheLittleGuyText1");

        // 4.
        MovieState* hitText1 = new MovieState
	  ("HitTheLittleGuyText1.mov", "HitTheLittleGuyText2");

        // 5.
        MovieState* hitText2 = new MovieState
	  ("HitTheLittleGuyText2.mov", "HitTheLittleGuyState");


        // 3. "Hit The Little Guy" (Simulator)
        HitTheLittleGuyState* hitState = new HitTheLittleGuyState
	  ("HitTheLittleGuySuccessState");
        hitState->SetNeedle(needleHandler);
        hitState->SetBackground(bg);

        // 4. Successfully accomplished (Video) 
        MovieState* hitSuccessState = new MovieState
	  ("HitTheLittleGuySuccess.mov", "SelectTheLittleGuyText");

        // 
        MovieState* selectText = new MovieState
	  ("SelectTheLittleGuyText.mov", "SelectTheLittleGuyState");

        // 5. "Select The Little Guy" (Simulator)
        SelectState* selectState = new SelectState
	  ("SelectTheLittleGuySuccessState");
        selectState->SetNeedle(needleHandler);
        selectState->SetBackground(bg);
        selectState->SetSpermatozoaList(hitState->GetSpermatozoaList());

        // 6. Successfully accomplished (Video)
        MovieState* selectSuccessState = new MovieState
	  ("SelectTheLittleGuySuccess.mov", "TurnTheEggText");

        //
        MovieState* turnText = new MovieState
	  ("TurnTheEggText.mov", "TurnTheEggState");

        // 7. "Turn The Egg" (Simulator)
        TurnTheEggState* turnState = new TurnTheEggState
	  ("TurnTheEggSuccessState", physic);
        turnState->SetNeedle(needleHandler);
        turnState->SetBackground(bg);

        // 8. Successfully accomplished (Video) 
        MovieState* turnSuccessState = new MovieState
	  ("TurnTheEggSuccess.mov", "InseminationText");

        // 9. Insemination (Simulator)
        MovieState* inseminateText = new MovieState
	  ("InseminationText.mov", "InseminationState");

        // 10. "Insemination" (Simulator)
        InseminateState* inseminateState = new InseminateState
	  ("InseminationSuccessState", physic);
        inseminateState->SetNeedle(needleHandler);
        inseminateState->SetBackground(bg);

        // 11. Successfully accomplished (Video)
        MovieState* inseminateSuccessState = new MovieState
	  ("InseminationSuccess.mov", "Outro");

        // 12. Outro (Video)
        MovieState* outro = new MovieState
	  ("Outro.mov", "StartupPicture", true);



        // Create and initialize StateManager
	StateManager* sm = new StateManager("StartupPicture", startup);

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

        engine.AddModule(*sm);



        // global quit event handlers
        TimedQuitEventHandler* tquit_h = new TimedQuitEventHandler(RESET_TIME);
	tquit_h->RegisterWithEngine(engine);

	GLScreenshot* sshot_h = new GLScreenshot(resourcedir + "screenshots/");
	sshot_h->RegisterWithEngine(engine);

        QuitHandler* quit_h = new QuitHandler();
	quit_h->BindToEventSystem();

	/*
	// Register the handler as a listener on up and down keyboard events.
        MoveHandler* move = new MoveHandler(*camera);
	move->RegisterWithEngine(engine);
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
    viewport->SetViewingVolume(camera);
      
    Renderer* renderer = new Renderer();
    renderer->AddRenderingView(new RenderingView(*viewport));
    this->renderer = renderer;
}

IFrame* Factory::GetFrame() { return frame; }

IRenderer* Factory::GetRenderer() { return renderer; }
