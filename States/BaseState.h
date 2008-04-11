#ifndef _BASE_STATE_
#define _BASE_STATE_

#include <Core/StateManager.h>

using OpenEngine::Core::StateManager;

class BaseState : public IModule {
private:
    string nextState;
    StateManager* sm;
protected:
    ISceneNode* root;

    BaseState() {}
    BaseState(string nextState) {
        this->nextState = nextState;
    }
public:
    virtual void Initialize() {
      (dynamic_cast<IMouse*>(IGameEngine::Instance().Lookup(typeid(IMouse))))
	->HideCursor();

        // Lookup the Game State Manager
        sm = dynamic_cast<StateManager*>(IGameEngine::Instance().Lookup(typeid(StateManager)));

        // Add physics to the scene graph
        IRenderer* rendere = dynamic_cast<IRenderer*>(IGameEngine::Instance().Lookup(typeid(IRenderer)));
        root = rendere->GetSceneRoot();
    }

    virtual void Deinitialize() {
    }

    virtual void Process(const float delta, const float percent) {
    }

    void NextState() {
        sm->ChangeState(nextState);
    }
};

#endif
