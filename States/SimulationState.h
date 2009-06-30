#ifndef _SIMULATION_STATE_
#define _SIMULATION_STATE_

#include "BaseState.h"

#include "Background.h"
#include "HUDisplay.h"
#include "NeedleHandler.h"


#include <Core/IListener.h>
#include <Devices/IKeyboard.h>
#include <Renderers/IRenderer.h>

using OpenEngine::Core::IListener;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;

class SimulationState : public BaseState, public IListener<KeyboardEventArg>,
public RenderNode {
private:
    float fade;
    float fadeTime;

protected:
    NeedleHandler* needleHandler;
    Background* bg;
    HUD::Surface* surface;
    bool changeState;

public:
    SimulationState(string nextState, StateObjects& so);
    void Initialize();
    void Deinitialize();
    void Process(ProcessEventArg arg);
    void SetNeedle(NeedleHandler* needle);
    void SetBackground(Background* bg);
    void Handle(KeyboardEventArg arg);
    void Accept(RenderingEventArg arg);
};

#endif
