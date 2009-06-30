#ifndef _STATE_OBJECTS_
#define _STATE_OBJECTS_

#include <Core/IEngine.h>
#include <Core/StateManager.h>
#include <Devices/IMouse.h>
#include <Devices/IKeyboard.h>
#include <Renderers/TextureLoader.h>
#include <Scene/ISceneNode.h>

#include <Display/HUD.h>

using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Display;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Renderers;

class StateObjects {
 private:
    ISceneNode* root;
    StateManager* sm;
    IMouse* mouse;
    IKeyboard& keyboard;
    IEngine& engine;
    TextureLoader& tl;
    HUD& hud;

 public:
 StateObjects(ISceneNode* root, StateManager* sm, IMouse* mouse
              ,IKeyboard& keyboard, IEngine& engine, TextureLoader& tl
              , HUD& hud) 
     : root(root), sm(sm), mouse(mouse), keyboard(keyboard)
        , engine(engine), tl(tl), hud(hud) {}

    ISceneNode* GetSceneNode() { return root; }
    StateManager* GetStateManager() { return sm; }
    IMouse* GetMouse() { return mouse; }
    IKeyboard& GetKeyboard() { return keyboard; }
    IEngine& GetEngine() { return engine; }
    TextureLoader& GetTextureLoader() { return tl; }
    HUD& GetHUD() { return hud; }
};

#endif // _STATE_OBJECTS_
