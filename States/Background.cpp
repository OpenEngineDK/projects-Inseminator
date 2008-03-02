// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Meta/OpenGL.h>
#include <Core/IModule.h>
#include <Core/IGameEngine.h>
#include <Resources/TGAResource.h>
#include <Resources/ITextureResource.h>
#include <Resources/ResourceManager.h>
#include <Renderers/OpenGL/TextureLoader.h>
#include <Renderers/IRenderer.h>
#include <Logging/Logger.h>
#include <Utils/Billboard.h>
#include "Background.h"

using namespace OpenEngine::Logging;
using namespace OpenEngine::Resources;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Renderers::OpenGL::TextureLoader;
using OpenEngine::Core::IGameEngine;

Background::Background(string texturefile) : background(NULL), root(NULL) {
  this->texturefile = texturefile;
}

Background::~Background(){
}

void Background::Initialize(){
    IRenderer* rendere = dynamic_cast<IRenderer*>(IGameEngine::Instance().Lookup(typeid(IRenderer)));
    root = rendere->GetSceneRoot();

    // Create static structures
    background = Billboard::Create(texturefile, 1920, 1200, 0.025);
    background->SetPosition(Vector<3,float>(0, 0, 12));
    root->AddNode(background);
}

void Background::Deinitialize(){
    root->RemoveNode(background);
}

void Background::Process(const float delta, const float percent){
    // Update the movement of the noisy crap
    list<TransformationNode*>::iterator itr;
    for(itr = noiseList.begin(); itr!=noiseList.end(); itr++){
        TransformationNode* trans = *itr;
        static float radian = 0.0;
        radian += 0.01;
        trans->SetPosition(Vector<3,float>(sin(radian), cos(radian), 0));
    }
}
