#ifndef _HUD_STATE_
#define _HUD_STATE_

#include "BaseState.h"
#include "HUDisplay.h"

class HUDState : public BaseState, public IListener<KeyboardEventArg> {
private:
    bool restart; //to restart the story board and start over

protected:
    HUDisplay* hud;

    HUDState() {}
    HUDState(string nextState, bool restart = false) : BaseState(nextState) {
        this->restart = restart;
        hud = NULL;
    }
public:
    virtual void Initialize() {
        BaseState::Initialize();

        // Subscribe for keyevents
        IKeyboard::keyEvent.Attach(*this);
        root->AddNode(hud);
    }

    virtual void Deinitialize() {
        IKeyboard::keyEvent.Detach(*this);
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

    void Handle(KeyboardEventArg arg) { 
        if( arg.sym == KEY_SPACE && arg.type == KeyboardEventArg::PRESS) {
	    hud->FadeDown();
        }
    }
};

#endif
