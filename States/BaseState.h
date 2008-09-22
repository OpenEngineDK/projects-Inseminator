#ifndef _BASE_STATE_
#define _BASE_STATE_

#include <Core/IState.h>
#include <Core/StateManager.h>
#include <Scene/ISceneNode.h>
#include <string>

#include "StateObjects.h"

using std::string;
using OpenEngine::Core::IState;
using OpenEngine::Core::StateManager;
using OpenEngine::Scene::ISceneNode;

class BaseState : public IState {
private:
    string nextState;
    StateManager* sm;
protected:
    StateObjects& so;
    ISceneNode* root;

    BaseState(string nextState, StateObjects& so);
    virtual ~BaseState();
public:
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Process(ProcessEventArg arg);
    void NextState();
};

#endif
