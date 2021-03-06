#ifndef _BASE_STATE_
#define _BASE_STATE_

#include <Core/IState.h>
#include <Core/StateManager.h>
#include <Scene/ISceneNode.h>
#include <string>

#include "StateObjects.h"

using namespace OpenEngine;
using OpenEngine::Core::IState;
using OpenEngine::Core::StateManager;
using OpenEngine::Scene::ISceneNode;

class BaseState : public IState {
private:
    std::string nextState;
    StateManager* sm;
protected:
    StateObjects& so;
    ISceneNode* root;

    BaseState(string nextState, StateObjects& so);
    virtual ~BaseState();
public:
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Process(Core::ProcessEventArg arg);
    void NextState();
};

#endif
