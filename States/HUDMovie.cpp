#include "HUDMovie.h"

#include <Resources/IMovieResource.h>
#include <Core/EngineEvents.h>
#include <Utils/Timer.h>

using namespace OpenEngine;
using namespace OpenEngine::Core;

HUDMovie::HUDMovie(IMovieResourcePtr movie, StateObjects& so) :
    HUDisplay(movie->GetMovieWidth(),movie->GetMovieHeight()), so(so) {
    mplayer = movie;
    reverseTexture = true;

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
    ((boost::shared_ptr<IListener<InitializeEventArg> >)
     mplayer)->Handle(InitializeEventArg());

    so.GetTextureReloader().ListenOn( mplayer->ChangedEvent() );    
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

    ((boost::shared_ptr<IListener<DeinitializeEventArg> >)
     mplayer)->Handle(DeinitializeEventArg());
}

void HUDMovie::Process(ProcessEventArg arg) {
    this->textureId = mplayer->GetID();
    if(mplayer->Ended()) endRequested = true;
    HUDisplay::Process(arg);
    
    ((boost::shared_ptr<IListener<ProcessEventArg> >)
     mplayer)->Handle(arg);
}
