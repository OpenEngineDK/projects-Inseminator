// Artificial Insemination Simulator.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _FACTORY_H_
#define _FACTORY_H_

//------------------- Settings ----------------------------
//static const int FRAME_WIDTH = 1680, FRAME_HEIGHT = 1050;
//static const int FRAME_WIDTH = 1440, FRAME_HEIGHT = 900;
static const int FRAME_WIDTH = 770, FRAME_HEIGHT = 450;
    
static const bool FULLSCREEN = false;
//static const bool FULLSCREEN = true;
    
static const int RESET_TIME = 20000; // in milli seconds
//---------------------------------------------------------

#include <Core/IGameFactory.h>
#include <Display/Camera.h>
#include <Display/Viewport.h>

using namespace OpenEngine::Core;
using namespace OpenEngine::Display;

class Factory : public IGameFactory {
private:  
  IFrame*    frame;
  IRenderer* renderer;
  Viewport*  viewport;
  Camera*    camera;
  
public:
  Factory();
  bool         SetupEngine(IGameEngine& engine);
  IFrame*      GetFrame();
  IRenderer*   GetRenderer();
};

#endif //_FACTORY_H_
