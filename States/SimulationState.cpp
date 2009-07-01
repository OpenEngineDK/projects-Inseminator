#include "SimulationState.h"

#include <Scene/PointLightNode.h>

static const bool skipKeyEnabled = false;

SimulationState::SimulationState(string nextState, StateObjects& so)
    : BaseState(nextState, so) {
    time = 0.0f;
    fadeTime = 850; //millisecs
    changeState = false;
    surface = NULL;

    light = new PointLightNode();
    Vector<4,float> color(0.0f, 0.0f, 0.0f, 1.0f);
    light->ambient = color;
    light->diffuse = light->specular = color; 
    lightPosition = new TransformationNode();
    lightPosition->SetPosition(Vector<3,float>(0.0, 0.0, -100.0));
    lightPosition->AddNode(light);
}

void SimulationState::Initialize() {
    BaseState::Initialize();

    root->AddNode(lightPosition);

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
}

void SimulationState::Deinitialize() {
    delete surface;

    needleHandler->Deinitialize();
    bg->Deinitialize();

    // Unsubscribe key events
    if(skipKeyEnabled)
        so.GetKeyboard().KeyEvent().Detach(*this);

    changeState = false; // reset changeState
    time = 0.0f; // reset fade
    
    BaseState::Deinitialize();
    needleHandler->Deinitialize(); //??? deinit again?
}

void SimulationState::Process(ProcessEventArg arg) {
    bg->Process(arg);
    needleHandler->Process(arg);

    float delta = arg.approx / 1000.0; // millisecs
    if (changeState) { // fade down
        if (time > 0.0) time -= delta;
        if (time <= 0.0) { // when faded down change to next state
            root->RemoveNode(lightPosition);
            NextState();
            return;
        }
    } else // fade up
        if (time < fadeTime) time += delta;

    if (time < 0.0) time = 0.0;
    else if (time > fadeTime) time = fadeTime;

    float fade = time / fadeTime;
    Vector<4,float> color(fade, fade, fade, 1.0f);
    light->ambient = color;
    light->diffuse = light->specular = color; 

    BaseState::Process(arg);
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
