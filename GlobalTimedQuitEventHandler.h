#ifndef _GLOBAL_TIMED_QUIT_EVENT_HANDLER_
#define _GLOBAL_TIMED_QUIT_EVENT_HANDLER_

#include "States/StateObjects.h"

#include <Core/IEngine.h>
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>

using OpenEngine::Core::IEngine;
using namespace OpenEngine::Devices;

class GlobalTimedQuitEventHandler : public IListener<Core::ProcessEventArg>
, public IListener<KeyboardEventArg>, public IListener<MouseMovedEventArg>
    , public IListener<MouseButtonEventArg> {
private:
    StateObjects& so;
    double timePast;
    unsigned int RESET_TIME;
    unsigned int counter;

public:
 GlobalTimedQuitEventHandler(unsigned int resetTime, StateObjects& so)
     : so(so), timePast(0.0f), counter(0) {
        RESET_TIME = resetTime;
    }

    void Handle(Core::ProcessEventArg arg) {
        float deltaTime = arg.approx/1000.0;
        timePast += deltaTime;

        if (timePast > RESET_TIME)
            so.ResetSystem();
    }

    void Handle(KeyboardEventArg arg) {
        timePast = 0.0f;
    }

    void Handle(MouseMovedEventArg arg) {
        // counter inserted to avoid resetting when mouse is
        // moved as part of initialization of simulation states
        counter++;
        if (counter >= 2) {
            timePast = 0.0f;
            counter = 0;
        }
    }

    void Handle(MouseButtonEventArg arg) {
        timePast = 0.0f;
    }
};

#endif
