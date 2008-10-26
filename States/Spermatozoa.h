#ifndef _SPERMATOZOA_H_
#define _SPERMATOZOA_H_

#include <Math/Vector.h>
#include <Scene/TransformationNode.h>
#include <Resources/IModelResource.h>
#include <string>

#include "StateObjects.h"

using std::string;
using OpenEngine::Math::Vector;
using OpenEngine::Scene::TransformationNode;
using OpenEngine::Resources::IModelResourcePtr;

class Spermatozoa {
private:
    StateObjects& so;
    bool healthy;
    bool alive;
    bool marked;
    float speed;
    float direction;
    TransformationNode* transNode;

public:
    Spermatozoa(StateObjects& so, bool healthy = true);
    ~Spermatozoa();
    
    void LoadTexture(string textureName);
    void SetSpeed(float speed);
    void SetPosition(Vector<3,float> pos);
    Vector<3,float> GetPosition();
    TransformationNode* GetTransformation();
    void SetTransformation(TransformationNode* trans);

    void Update(const float delta);
    bool IsHealthy() { return healthy; }
    bool IsAlive() { return alive; }
    bool IsMarked() { return marked; }

    void Mark() { marked = true; }
    void Kill() { alive = false; }

};

#endif
