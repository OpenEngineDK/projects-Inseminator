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

#include <string>

// forward declarations
namespace OpenEngine {
    namespace Core {
        class IEngine;
    }
    namespace Display {
        class Camera;
        class IFrame;
        class Viewport;
    }
  namespace Renderers {
      class IRenderer;
  }
}

using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers;

class Factory {
private:  
  IFrame*    frame;
  IRenderer* renderer;
  Viewport*  viewport;
  Camera*    camera;
  
public:
  Factory();
  virtual ~Factory();
  bool         SetupEngine(IEngine& engine, std::string startState);
  IFrame*      GetFrame();
  IRenderer*   GetRenderer();
};

#endif //_FACTORY_H_
