#include "SimulationState.h"

static const bool skipKeyEnabled = false;

SimulationState::SimulationState() {}

SimulationState::SimulationState(string nextState) : BaseState(nextState) {
        fade = 0.0f;
        fadeTime = 0.001;
        changeState = false;
}

void SimulationState::Initialize() {
    BaseState::Initialize();

    // Subscribe for keyevents
    if(skipKeyEnabled)
        IKeyboard::keyEvent.Attach(*this);

    // Initialize all
    bg->Initialize();
    needleHandler->Initialize(); 

    // Load texture
    ITextureResourcePtr texture =
        ResourceManager<ITextureResource>::Create("MicroHUD.tga"); 
    TextureLoader::LoadTextureResource(texture);
    hud = new HUDisplay(texture);
    hud->Blend(true);
    root->AddNode(hud);
}

void SimulationState::Deinitialize() {
    root->RemoveNode(hud);

    needleHandler->Deinitialize();
    bg->Deinitialize();
    
    // Unsubscribe key events
    if(skipKeyEnabled)
        IKeyboard::keyEvent.Detach(*this);
    
    changeState = false; // reset changeState
    fade = 0.0f; // reset fade
    
    BaseState::Deinitialize();
    needleHandler->Deinitialize(); //??? deinit again?
}

void SimulationState::Process(const float delta, const float percent) {
    bg->Process(delta, percent);
    needleHandler->Process(delta, percent);
    hud->Process(delta,percent);

    if (changeState) { // fade down
        if (fade > 0.0) fade -= fadeTime * delta;
	if (fade < 0.000001) { // when faded down change to next state
	    NextState();
	    return;
	}
    } else // fade up
        if (fade < 1.0) fade += fadeTime * delta;
    glClearColor(fade, fade, fade, 0.5f);

    BaseState::Process(delta,percent);
}

void SimulationState::SetNeedle(NeedleHandler* needle) {
    this->needleHandler = needle;
}

void SimulationState::SetBackground(Background* bg) {
    this->bg = bg;
}

void SimulationState::Handle(KeyboardEventArg arg) { 
    if( arg.sym == KEY_SPACE && arg.type == KeyboardEventArg::PRESS) {
        changeState = true;
    }
}
