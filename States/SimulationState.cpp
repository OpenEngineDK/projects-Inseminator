#include "SimulationState.h"

static const bool skipKeyEnabled = false;

SimulationState::SimulationState(string nextState, StateObjects& so) : BaseState(nextState, so) {
        fade = 0.0f;
        fadeTime = 0.001;
        changeState = false;
        surface = NULL;
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
        ResourceManager<ITextureResource>::Create("MicroHUD-reverted-withalpha.tga"); 
    so.GetTextureLoader().Load(texture);
    
    HUD& hud = so.GetHUD();
    surface = hud.CreateSurface(texture);
    surface->SetScale(HUD::Surface::FULLSCREEN);

    root->AddNode(this);
}

void SimulationState::Deinitialize() {
    root->RemoveNode(this);
    delete surface;

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

    float delta = arg.approx / 1000.0;
    if (changeState) { // fade down
        if (fade > 0.0) fade -= fadeTime * delta;
        if (fade < 0.000001) { // when faded down change to next state
            NextState();
            return;
        }
    } else // fade up
        if (fade < 1.0) fade += fadeTime * delta;
    BaseState::Process(arg);
}

void SimulationState::Accept(RenderingEventArg arg) {
    // @todo: does this do anything???
    Vector<4,float> color(fade, fade, fade, 0.5f);
    arg.renderer.SetBackgroundColor(color);
}

void SimulationState::SetNeedle(NeedleHandler* needle) {
    this->needleHandler = needle;
}

void SimulationState::SetBackground(Background* bg) {
    this->bg = bg;
}

void SimulationState::Handle(KeyboardEventArg arg) { 
    if( arg.sym == KEY_SPACE && arg.type == EVENT_PRESS) {
        changeState = true;
    }
}
