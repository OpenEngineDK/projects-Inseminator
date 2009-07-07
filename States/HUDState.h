#ifndef _HUD_STATE_
#define _HUD_STATE_

#include "BaseState.h"
#include "HUDisplay.h"
#include "StateObjects.h"

class HUDState : public BaseState, public IListener<KeyboardEventArg> {
private:
    bool restart; //to restart the story board and start over

protected:
    HUDisplay* hud;

 HUDState(string nextState, StateObjects& so, bool restart = false) 
     : BaseState(nextState, so) {
        this->restart = restart;
        hud = NULL;
    }
public:
    virtual void Initialize() {
        BaseState::Initialize();

        // Subscribe for keyevents
        so.GetKeyboard().KeyEvent().Attach(*this);
        root->AddNode(hud);
    }

    virtual void Deinitialize() {
        so.GetKeyboard().KeyEvent().Detach(*this);
        root->RemoveNode(hud);

        BaseState::Deinitialize();
    }

    virtual void Process(ProcessEventArg arg) {
        BaseState::Process(arg);

        if (hud->Ended()) {
            if (restart)
                so.ResetSystem();
            else
                NextState();
        }
        
        hud->Process(arg);
    }

    void Handle(KeyboardEventArg arg) { 
        if( arg.sym == KEY_SPACE && arg.type == EVENT_PRESS) {
            hud->FadeDown();
        }
    }
};

#endif
