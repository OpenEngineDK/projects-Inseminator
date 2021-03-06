#ifndef _SIMULATION_STATE_
#define _SIMULATION_STATE_

#include "BaseState.h"

#include "Background.h"
#include "HUDisplay.h"
#include "NeedleHandler.h"


#include <Core/IListener.h>
#include <Devices/IKeyboard.h>
#include <Renderers/IRenderer.h>
#include <Scene/PointLightNode.h>

using OpenEngine::Core::IListener;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;

class SimulationState : public BaseState, public IListener<KeyboardEventArg> {
private:
    float time;
    float fadeTime;
    Scene::PointLightNode* light;
    TransformationNode* lightPosition;

protected:
    NeedleHandler* needleHandler;
    Background* bg;
    HUD::Surface* surface;
    bool changeState;
    float stateClock;

public:
    SimulationState(string nextState, StateObjects& so);
    void Initialize();
    void Deinitialize();
    void Process(Core::ProcessEventArg arg);
    void SetNeedle(NeedleHandler* needle);
    void SetBackground(Background* bg);
    void Handle(KeyboardEventArg arg);
};

#endif
