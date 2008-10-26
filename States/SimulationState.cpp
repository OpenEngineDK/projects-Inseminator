#include "SimulationState.h"

static const bool skipKeyEnabled = false;

SimulationState::SimulationState(string nextState, StateObjects& so) : BaseState(nextState, so) {
        fade = 0.0f;
        fadeTime = 0.001;
        changeState = false;
}

void SimulationState::Initialize() {
    BaseState::Initialize();

    // Subscribe for keyevents
    if(skipKeyEnabled)
        so.GetKeyboard().KeyEvent().Attach(*this);

    // Initialize all
    bg->Initialize();
    needleHandler->Initialize(); 

    // Load texture
    ITextureResourcePtr texture =
        ResourceManager<ITextureResource>::Create("MicroHUD.tga"); 
    so.GetTextureLoader().Load(texture);
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
        so.GetKeyboard().KeyEvent().Detach(*this);

    changeState = false; // reset changeState
    fade = 0.0f; // reset fade
    
    BaseState::Deinitialize();
    needleHandler->Deinitialize(); //??? deinit again?
}

void SimulationState::Process(ProcessEventArg arg) {
    bg->Process(arg);
    needleHandler->Process(arg);
    hud->Process(arg);

    float delta = arg.approx / 1000.0;
    if (changeState) { // fade down
        if (fade > 0.0) fade -= fadeTime * delta;
        if (fade < 0.000001) { // when faded down change to next state
            NextState();
            return;
        }
    } else // fade up
        if (fade < 1.0) fade += fadeTime * delta;
    glClearColor(fade, fade, fade, 0.5f);

    BaseState::Process(arg);
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
