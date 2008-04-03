#include "HUDMovie.h"

HUDMovie::HUDMovie(IMovieResourcePtr movie) :
  HUDisplay(movie->GetMovieWidth(),movie->GetMovieHeight()) {
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
    mplayer->Initialize(); // sets the texid if not already there
    this->textureId = mplayer->GetID();
    mplayer->Restart();
    mplayer->Pause(false);

    blend = false;
    hasInitialized = false;
    endRequested = false;
    fade = 1.0f;
}

void HUDMovie::Deinitialize() {
    mplayer->Pause(true);
}

void HUDMovie::Process(const float delta, const float percent) {
  if(mplayer->Ended()) endRequested = true;
  HUDisplay::Process(delta,percent);
  mplayer->Process(delta, percent);
}
