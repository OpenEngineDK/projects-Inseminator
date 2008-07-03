#ifndef _BASE_STATE_
#define _BASE_STATE_

#include <Core/IModule.h>
#include <Core/StateManager.h>
#include <Scene/ISceneNode.h>
#include <string>

using std::string;
using OpenEngine::Core::IModule;
using OpenEngine::Core::StateManager;
using OpenEngine::Scene::ISceneNode;

class BaseState : public IModule {
private:
    string nextState;
    StateManager* sm;
protected:
    ISceneNode* root;

    BaseState();
    BaseState(string nextState);
    virtual ~BaseState();
public:
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Process(const float delta, const float percent);
    void NextState();
};

#endif
