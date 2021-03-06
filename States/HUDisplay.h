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

#include <Core/EngineEvents.h>
#include <Scene/RenderNode.h>
#include <Renderers/IRenderingView.h>
#include <Resources/ITexture2D.h>

using namespace OpenEngine;
using namespace OpenEngine::Core;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;

class HUDisplay : public RenderNode {
protected:
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    bool hasInitialized, endRequested, blend, reverseTexture;
    int textureId;
    float fade, maxX, maxY;
public:
    HUDisplay(unsigned int width, unsigned int height);
    HUDisplay(ITexture2DPtr texture);
    ~HUDisplay();

    void Apply(Renderers::RenderingEventArg arg, ISceneNodeVisitor& v);

    // Override this for special use
    virtual void DrawInOrthoMode();

    void ApplyOrthoView();
    void ApplyProjectionView();

    bool Ended();
    void FadeDown();
    void Blend(bool blend);
    void Process(Core::ProcessEventArg arg);
};

#endif
