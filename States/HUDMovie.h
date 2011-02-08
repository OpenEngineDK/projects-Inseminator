// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _HEADS_UP_DISPLAY_MOVIE_H_
#define _HEADS_UP_DISPLAY_MOVIE_H_

#include <Scene/RenderNode.h>
#include "HUDisplay.h"
#include <Resources/IMovieResource.h>
#include "StateObjects.h"

using OpenEngine::Scene::RenderNode;
using OpenEngine::Renderers::IRenderer;
using namespace OpenEngine::Resources;

class HUDMovie : public HUDisplay {
private:
    IMovieResourcePtr mplayer;
    StateObjects& so;

public:
    HUDMovie(IMovieResourcePtr mplayer, StateObjects& so, bool reverseTexture);
    ~HUDMovie();

    // Only for processing the movie decoding
    void Process(Core::ProcessEventArg arg);

    void Initialize();
    void Deinitialize();
};

#endif
