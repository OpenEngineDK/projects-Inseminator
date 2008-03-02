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

#include <Renderers/IRenderNode.h>
#include "HUDisplay.h"
#include <Resources/MovieResource.h>

using OpenEngine::Scene::IRenderNode;
using OpenEngine::Renderers::IRenderer;

class HUDMovie : public HUDisplay {
private:
    MovieResource* mplayer;

public:
    HUDMovie(MovieResource* mplayer);
    ~HUDMovie();

    // Only for processing the movie decoding
    void Process(const float delta, const float percent);

    void Initialize();
    void Deinitialize();
};

#endif
