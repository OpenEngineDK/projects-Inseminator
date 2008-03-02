#include "Spermatozoa.h"
#include <Utils/Billboard.h>
#include <cstdlib> 
#include <ctime> 

using namespace std;

Spermatozoa::Spermatozoa(bool healthy) : 
    healthy(healthy),alive(true),marked(false),speed(-0.001f),direction(0),transNode(NULL){
    // The little guy is dead by 30 percent chance
    alive = (rand()/(float)RAND_MAX) < 0.3f ? false : true;
    // Choose direction -1 = left, 0 = straight, 1 = right.
    direction = (rand()/(float)RAND_MAX) > 0.5f ? -1 : 1;
}

Spermatozoa::~Spermatozoa(){
}

void Spermatozoa::LoadTexture(string textureName) {
    transNode = Billboard::Create(textureName, 119, 22, 0.01);
}

void Spermatozoa::SetSpeed(float speed) {
    this->speed = speed;
}

void Spermatozoa::SetPosition(Vector<3,float> pos) {
    if( transNode != NULL )
        transNode->SetPosition(pos);
}
    
Vector<3,float> Spermatozoa::GetPosition() {
    return transNode->GetPosition();
}

TransformationNode* Spermatozoa::GetTransformation() {
    return transNode;
}

void Spermatozoa::SetTransformation(TransformationNode* trans) {
    this->transNode = trans;
}
    
void Spermatozoa::Update(const float delta) {
    if( transNode == NULL || !alive) return;

    float angle = 0;
    // Choose angle between 5 and -5 degrees
    angle = ((rand()/(float)RAND_MAX)*20) - 10;
    angle = angle * (PI/180);
    
    // Turn the same way as direction
    if( angle < 0 && direction > 0 ) angle *= -1;
    if( angle > 0 && direction < 0 ) angle *= -1;
    
    // Change direction? 
    if( (rand()/(float)RAND_MAX) > 0.95f ) direction *= -1;
    
    transNode->Rotate(0,0,(angle/10)*delta);
    transNode->Move(speed*delta,0,0);
}
