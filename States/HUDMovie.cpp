#include "HUDMovie.h"

#include <Resources/IMovieResource.h>
#include <Core/EngineEvents.h>
#include <Utils/Timer.h>

using namespace OpenEngine;
using namespace OpenEngine::Core;

HUDMovie::HUDMovie(IMovieResourcePtr movie, StateObjects& so, bool reverseTex) :
    HUDisplay(movie->GetMovieWidth(),movie->GetMovieHeight()), so(so) {
    reverseTexture = reverseTex;
    mplayer = movie;

    blend = false;
    hasInitialized = false;
    endRequested = false;
    fade = 1.0f;

    maxX = (float)mplayer->GetMovieWidth() / (float)mplayer->GetWidth();
    maxY = (float)mplayer->GetMovieHeight() / (float)mplayer->GetHeight();
}

HUDMovie::~HUDMovie() {}

void HUDMovie::Initialize() {
    // sets the texid if not already there
    ((boost::shared_ptr<IListener<Core::InitializeEventArg> >)
     mplayer)->Handle(Core::InitializeEventArg());

    so.GetTextureLoader().Load( mplayer, TextureLoader::RELOAD_IMMEDIATE );    
    mplayer->Restart();
    mplayer->Pause(false);

    blend = false;
    hasInitialized = false;
    endRequested = false;
    fade = 1.0f;
}

using namespace OpenEngine::Resources;
void HUDMovie::Deinitialize() {
    mplayer->Pause(true);

    ((boost::shared_ptr<IListener<Core::DeinitializeEventArg> >)
     mplayer)->Handle(Core::DeinitializeEventArg());
}

void HUDMovie::Process(Core::ProcessEventArg arg) {
    this->textureId = mplayer->GetID();
    if(mplayer->Ended()) endRequested = true;
    HUDisplay::Process(arg);
    
    ((boost::shared_ptr<IListener<Core::ProcessEventArg> >)
     mplayer)->Handle(arg);
}
