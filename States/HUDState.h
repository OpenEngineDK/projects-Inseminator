#ifndef _HUD_STATE_
#define _HUD_STATE_

#include "BaseState.h"
#include "HUDisplay.h"

class HUDState : public BaseState {
private:
    bool restart; //to restart the story board and start over
    Listener<HUDState,KeyboardEventArg>* down;
protected:
    HUDisplay* hud;

    HUDState() {}
    HUDState(string nextState, bool restart = false) : BaseState(nextState) {
        this->restart = restart;
        hud = NULL;
        down = new Listener<HUDState,KeyboardEventArg>
	  (*this, &HUDState::HandleDownEvent);
    }
public:
    virtual void Initialize() {
        BaseState::Initialize();

        // Subscribe for keyevents
        IKeyboard::keyDownEvent.Add(down);
        root->AddNode(hud);
    }

    virtual void Deinitialize() {
        IKeyboard::keyDownEvent.Remove(down);
        root->RemoveNode(hud);

        BaseState::Deinitialize();
    }

    virtual void Process(const float delta, const float percent) {
	BaseState::Process(delta,percent);

	if (hud->Ended()) {
	  if (restart) {
	    Deinitialize();
	    IGameEngine::Instance().Stop();
	  }
	  else
	    NextState();
	}

        hud->Process(delta, percent);
    }

    void HandleDownEvent(KeyboardEventArg arg) { 
        if( arg.sym == KEY_SPACE ){
	    hud->FadeDown();
        }
    }
};

#endif
