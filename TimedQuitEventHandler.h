#ifndef _TIMED_QUIT_EVENT_HANDLER_
#define _TIMED_QUIT_EVENT_HANDLER_

#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>

using namespace OpenEngine::Devices;

class TimedQuitEventHandler : public IModule {
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
        Listener<TimedQuitEventHandler, KeyboardEventArg>* quit_down
            = new Listener<TimedQuitEventHandler, KeyboardEventArg>
	  (*this, &TimedQuitEventHandler::HandleDownEvent);
        IKeyboard::keyDownEvent.Add(quit_down);

        Listener<TimedQuitEventHandler, KeyboardEventArg>* quit_up
            = new Listener<TimedQuitEventHandler, KeyboardEventArg>
	  (*this, &TimedQuitEventHandler::HandleUpEvent);
        IKeyboard::keyUpEvent.Add(quit_up);

        engine.AddModule(*this);
    }
};

#endif
