// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Core/IModule.h>
#include <Resources/ITextureResource.h>
#include <Resources/ResourceManager.h>
#include <Renderers/TextureLoader.h>
#include <Renderers/IRenderer.h>
#include <Logging/Logger.h>
#include <Utils/Billboard.h>
#include "Background.h"

using namespace OpenEngine::Logging;
using namespace OpenEngine::Resources;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Renderers::TextureLoader;

Background::Background(string texturefile, ISceneNode* root) : background(NULL), root(root) {
  this->texturefile = texturefile;
}

Background::~Background(){
}

void Background::Initialize(){
    // Create static structures
    background = Billboard::Create(texturefile, 1920, 1200, 0.025);
    background->SetPosition(Vector<3,float>(0, 0, 12));
    root->AddNode(background);
}

void Background::Deinitialize(){
    root->RemoveNode(background);
}

void Background::Process(ProcessEventArg arg) {
    /* @todo: old lets overs, remove Process method

    // Update the movement of the noisy crap
    list<TransformationNode*>::iterator itr;
    for(itr = noiseList.begin(); itr!=noiseList.end(); itr++){
        TransformationNode* trans = *itr;
        static float radian = 0.0;
        radian += 0.01;
        trans->SetPosition(Vector<3,float>(sin(radian), cos(radian), 0));
    }
    */
}
