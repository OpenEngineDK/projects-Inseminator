#ifndef _MOVIE_STATE_
#define _MOVIE_STATE_

#include <Core/IModule.h>
#include <Devices/IKeyboard.h>
#include <Logging/Logger.h>
#include <Resources/ResourceManager.h>

#include "HUDMovie.h"
#include "HUDState.h"

using OpenEngine::Logging::Logger;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Core;

class MovieState : public HUDState {
private:
    HUDMovie* movie;

public:
 MovieState(IMovieResourcePtr mplayer, string nextState, 
            bool reverseTexture, StateObjects& so, 
            bool restart = false) 
     : HUDState(nextState, so, restart) {
        movie = new HUDMovie(mplayer, so, reverseTexture);
        hud = movie;
    }
    ~MovieState() {}

    virtual void Initialize(){
        movie->Initialize();
        HUDState::Initialize();
    }
    
    virtual void Deinitialize() {
	HUDState::Deinitialize();
        movie->Deinitialize();
    }

    virtual void Process(Core::ProcessEventArg arg){
        HUDState::Process(arg);
        movie->Process(arg);
    }
};

#endif
