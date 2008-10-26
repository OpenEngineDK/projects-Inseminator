#ifndef _PICTURE_STATE_
#define _PICTURE_STATE_

#include <Core/IModule.h>
#include <Devices/IKeyboard.h>
#include <Devices/IMouse.h>
#include <Logging/Logger.h>
#include <Renderers/TextureLoader.h>
#include "HUDisplay.h"

#include "HUDState.h"

using OpenEngine::Logging::Logger;
using OpenEngine::Renderers::TextureLoader;
using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;

class PictureState : public HUDState {
private:
    string pictureName;
    MouseState initMouseState;
    IMouse* mouse;

public:
 PictureState(string pictureName, string nextState, StateObjects& so)
     : HUDState(nextState, so) {
        this->mouse = so.GetMouse();
        this->pictureName = pictureName;
    }
    ~PictureState() {}

    virtual void Initialize() {
        //mouse = dynamic_cast<IMouse*>(IGameEngine::Instance().Lookup(typeid(IMouse)));
        initMouseState = mouse->GetState();

        // Load texture
        ITextureResourcePtr texture =
	  ResourceManager<ITextureResource>::Create(pictureName); 
	so.GetTextureLoader().Load(texture);
        hud = new HUDisplay(texture);

        HUDState::Initialize();
    }
    
    virtual void Deinitialize() {
	HUDState::Deinitialize();
    }

    virtual void Process(ProcessEventArg arg) {
        //wait for mouse movement to go to the next state
        MouseState currentMouseState = mouse->GetState();
        if(initMouseState.x==0 && initMouseState.y == 0)
            initMouseState = mouse->GetState(); //@todo this is done because the initState is not initialized before process
        else if ( (abs(initMouseState.x - currentMouseState.x) > 5) || (abs(initMouseState.y != currentMouseState.y) > 5) )
            hud->FadeDown();

	HUDState::Process(arg);
    }
};

#endif
