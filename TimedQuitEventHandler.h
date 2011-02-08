#ifndef _TIMED_QUIT_EVENT_HANDLER_
#define _TIMED_QUIT_EVENT_HANDLER_

#include "States/StateObjects.h"

#include <Core/IEngine.h>
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>

using OpenEngine::Core::IEngine;
using namespace OpenEngine::Devices;

class TimedQuitEventHandler : public IListener<Core::ProcessEventArg>
, public IListener<KeyboardEventArg>  {
private:
    StateObjects& so;
    double timePast;
    bool left, right;
    int RESET_TIME;

public:
 TimedQuitEventHandler(int resetTime, StateObjects& so) : so(so) {
        timePast = 0.0f;
        left = right = false;
        RESET_TIME = resetTime;
    }

    void Handle(Core::ProcessEventArg arg) {
        float deltaTime = arg.approx/1000.0;
        if (left && right)
            timePast += deltaTime;
        else
            timePast = 0.0f;

        if (timePast > RESET_TIME)
            so.ResetSystem();
    }

    void Handle(KeyboardEventArg arg) {
        if (arg.type == EVENT_PRESS)
            HandleDownEvent(arg);
        else
            HandleUpEvent(arg);
    }

    void HandleUpEvent(KeyboardEventArg arg) {
        if (arg.sym == KEY_LEFT)
            left = false;
        else if (arg.sym == KEY_RIGHT)
            right = false;
    }

    void HandleDownEvent(KeyboardEventArg arg) {
        if (arg.sym == KEY_LEFT)
            left = true;
        else if (arg.sym == KEY_RIGHT)
            right = true;
    }
};

#endif
