#ifndef _STATE_OBJECTS_
#define _STATE_OBJECTS_

#include <Core/IEngine.h>
#include <Core/StateManager.h>
#include <Devices/IMouse.h>
#include <Devices/IKeyboard.h>
#include <Renderers/TextureLoader.h>
#include <Scene/ISceneNode.h>

using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;
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

 public:
 StateObjects(ISceneNode* root, StateManager* sm, IMouse* mouse
              ,IKeyboard& keyboard, IEngine& engine, TextureLoader& tl) 
     : root(root), sm(sm), mouse(mouse), keyboard(keyboard)
        , engine(engine), tl(tl) {}

    ISceneNode* GetSceneNode() { return root; }
    StateManager* GetStateManager() { return sm; }
    IMouse* GetMouse() { return mouse; }
    IKeyboard& GetKeyboard() { return keyboard; }
    IEngine& GetEngine() { return engine; }
    TextureLoader& GetTextureLoader() { return tl; }
};

#endif // _STATE_OBJECTS_
