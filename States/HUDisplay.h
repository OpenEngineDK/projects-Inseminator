// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _HEADS_UP_DISPLAY_H_
#define _HEADS_UP_DISPLAY_H_

#include <Renderers/IRenderNode.h>
#include <Renderers/IRenderingView.h>

#include <Resources/MovieResource.h>

using namespace OpenEngine::Renderers;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 600;

class HUDisplay : public IRenderNode {
protected:
  bool hasInitialized, endRequested, blend, reverseTexture;
    int textureId;
    float fade, maxX, maxY;
public:
    HUDisplay();
    HUDisplay(int textureId);
    ~HUDisplay();

    void Apply(IRenderingView* rende);

    // Override this for special use
    virtual void DrawInOrthoMode();

    void ApplyOrthoView();
    void ApplyProjectionView();

    bool Ended();
    void FadeDown();
    void Blend(bool blend);
    void Process(const float delta, const float percent);
};

#endif
