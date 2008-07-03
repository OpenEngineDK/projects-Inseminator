#include "BaseState.h"

#include <Core/IGameEngine.h>
#include <Devices/IMouse.h>
#include <Renderers/IRenderer.h>

using OpenEngine::Devices::IMouse;
using OpenEngine::Core::IGameEngine;
using OpenEngine::Renderers::IRenderer;

BaseState::BaseState() {}

BaseState::~BaseState() {}

BaseState::BaseState(string nextState) {
    this->nextState = nextState;
}

void BaseState::Initialize() {
    (dynamic_cast<IMouse*>(IGameEngine::Instance().Lookup(typeid(IMouse))))
      ->HideCursor();

    // Lookup the Game State Manager
    sm = dynamic_cast<StateManager*>(IGameEngine::Instance().Lookup(typeid(StateManager)));

    // Add physics to the scene graph
    IRenderer* rendere = dynamic_cast<IRenderer*>(IGameEngine::Instance().Lookup(typeid(IRenderer)));
    root = rendere->GetSceneRoot();
}

void BaseState::Deinitialize() {
}

    
void BaseState::Process(const float delta, const float percent) {
}

void BaseState::NextState() {
    sm->ChangeState(nextState);
}
