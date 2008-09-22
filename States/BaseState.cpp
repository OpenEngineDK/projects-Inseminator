#include "BaseState.h"

//#include <Core/IGameEngine.h>
#include <Devices/IMouse.h>
#include <Renderers/IRenderer.h>

using OpenEngine::Devices::IMouse;
//using OpenEngine::Core::IGameEngine;
using OpenEngine::Renderers::IRenderer;

BaseState::~BaseState() {}

BaseState::BaseState(string nextState, StateObjects& so) : so(so) {
    this->root = so.GetSceneNode();
    this->sm = so.GetStateManager();
    this->nextState = nextState;
}

void BaseState::Initialize() {
    so.GetMouse()->HideCursor();
}

void BaseState::Deinitialize() {
}

    
void BaseState::Process(ProcessEventArg arg) {
}

void BaseState::NextState() {
    sm->ChangeState(nextState);
}
