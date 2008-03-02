#ifndef _MOVIE_STATE_
#define _MOVIE_STATE_

#include <Core/IModule.h>
#include <Core/IGameEngine.h>
#include <Devices/IKeyboard.h>
#include <Logging/Logger.h>

#include "HUDMovie.h"
#include "HUDState.h"

using OpenEngine::Logging::Logger;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Core;

class MovieState : public HUDState {
private:
    HUDMovie* movie;

public:
 MovieState(string movieName, string nextState, bool restart = false) : HUDState(nextState, restart) {
        MovieResource* mplayer = new MovieResource(movieName, false);
        movie = new HUDMovie(mplayer);
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

    virtual void Process(const float delta, const float percent){
        HUDState::Process(delta,percent);
        movie->Process(delta,percent);
    }

    bool IsTypeOf(const std::type_info& inf) {
        return typeid(MovieState) == inf;
    }
};

#endif
