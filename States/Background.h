// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <Core/IState.h>
#include <Scene/ISceneNode.h>
#include <Scene/TransformationNode.h>
#include <list>

using std::list;
using OpenEngine::Core::IState;
using OpenEngine::Scene::TransformationNode;
using OpenEngine::Scene::ISceneNode;

class Background : public IState {
private:
    //list<TransformationNode*> noiseList;
    TransformationNode* background;
    ISceneNode* root;
    string texturefile;
    
public:
    Background(string texturefile, ISceneNode* root);
    ~Background();

    void Initialize();
    void Deinitialize();
    void Process(ProcessEventArg arg);
};

#endif
