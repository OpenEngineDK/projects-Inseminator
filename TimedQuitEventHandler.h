#ifndef _TIMED_QUIT_EVENT_HANDLER_
#define _TIMED_QUIT_EVENT_HANDLER_

#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>

using namespace OpenEngine::Devices;

class TimedQuitEventHandler : public IModule, public IListener<KeyboardEventArg>  {
private:
    double timePast;
    bool left, right;
    int RESET_TIME;

    void ResetSystem() {
      /* this resets the system because the OS
	 restarts the program when it exits */
      IGameEngine::Instance().Stop();

      /*
	// old method
	GameStateManager* gsm = dynamic_cast<GameStateManager*>
	  (IGameEngine::Instance().Lookup(typeid(GameStateManager)));
	gsm->ChangeState("StartupPicture"); // restart the simulator
      */
    }

public:
    TimedQuitEventHandler(int resetTime) {
      timePast = 0.0f;
      left = right = false;
      RESET_TIME = resetTime;
    }

    void Initialize() {}
    void Deinitialize() {}
    bool IsTypeOf(const std::type_info& inf) { return false; }

    void Process(const float deltaTime, const float percent) {
      if (left && right)
	timePast += deltaTime;
      else
	timePast = 0.0f;
      
      if (timePast > RESET_TIME)
	ResetSystem();
    }

    void Handle(KeyboardEventArg arg) {
      if (arg.type == KeyboardEventArg::PRESS)
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

    void RegisterWithEngine(IGameEngine& engine) {
        IKeyboard::keyEvent.Attach(*this);
        engine.AddModule(*this);
    }
};

#endif
