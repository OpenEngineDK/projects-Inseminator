#ifndef _SIMULATION_STATE_
#define _SIMULATION_STATE_

#include "BaseState.h"

static const bool skipKeyEnabled = false;

class SimulationState : public BaseState {
private:
    float fade;
    float fadeTime;
    Listener<SimulationState,KeyboardEventArg>* down;

protected:
    NeedleHandler* needleHandler;
    Background* bg;
    HUDisplay* hud;

    bool changeState;

    SimulationState() {}
 SimulationState(string nextState) : BaseState(nextState) {
        fade = 0.0f;
        fadeTime = 0.001;
        changeState = false;

	if(skipKeyEnabled) {
	    down = new Listener<SimulationState,KeyboardEventArg>
	      (*this, &SimulationState::HandleDownEvent);
	}
    }
public:
    void Initialize() {
        BaseState::Initialize();

        // Subscribe for keyevents
	if(skipKeyEnabled)
	    IKeyboard::keyDownEvent.Add(down);

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

    void Deinitialize() {
        root->RemoveNode(hud);

        needleHandler->Deinitialize();
        bg->Deinitialize();

        // Unsubscribe key events
	if(skipKeyEnabled)
	    IKeyboard::keyDownEvent.Remove(down);

        changeState = false; // reset changeState
	fade = 0.0f; // reset fade

        BaseState::Deinitialize();
        needleHandler->Deinitialize(); //??? deinit again?
    }

    void Process(const float delta, const float percent) {
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

    void SetNeedle(NeedleHandler* needle){
        this->needleHandler = needle;
    }

    void SetBackground(Background* bg){
        this->bg = bg;
    }

    void HandleDownEvent(KeyboardEventArg arg) { 
        if (arg.sym == KEY_SPACE)
            changeState = true;
    }
};

#endif
