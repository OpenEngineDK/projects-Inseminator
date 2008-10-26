#ifndef _HIT_THE_LITTLE_GUY_STATE_H_
#define _HIT_THE_LITTLE_GUY_STATE_H_

#include "NeedleHandler.h"
#include "Spermatozoa.h"
#include "SimulationState.h"

const float SPERM_SPEED = -0.0007f;
const int NUM_SPERMATOZOA = 6;
const int SEED = 0;

class HitTheLittleGuyState : public SimulationState {
private:
    list<Spermatozoa*>* spermList;
    Spermatozoa* spermMarked;

public:
 HitTheLittleGuyState(string nextState, StateObjects& so)
     : SimulationState(nextState, so) {
        spermList = new list<Spermatozoa*>();
    }
    ~HitTheLittleGuyState() {}
    list<Spermatozoa*>* GetSpermatozoaList() { return spermList; }

    void Initialize() {
        SimulationState::Initialize();

        srand((unsigned)SEED);
        // Create list of spermatozoa's
        spermList->clear();
        for( int i=0; i<NUM_SPERMATOZOA; i++ )
            for( int j=0; j<NUM_SPERMATOZOA; j++ ){
	        Spermatozoa* littleGuy = new Spermatozoa(so,true);
                float random = (rand()/(float)RAND_MAX)*10;
                float speed = SPERM_SPEED + (SPERM_SPEED/(float)10 * (random));
                littleGuy->SetSpeed(speed);
                littleGuy->LoadTexture("SpermatozoaNormal.tga");
                // Generate random numbers between 0 and 1 with 2 digits
                float randX = (rand()/(float)RAND_MAX);
                float randY = (rand()/(float)RAND_MAX);
                littleGuy->GetTransformation()->SetRotation(Quaternion<float>(Vector<3,float>(randX-0.5,randY-0.5,0).GetNormalize()));
                littleGuy->SetPosition(Vector<3,float>(4-j+randX, 3-i+randY,-0.1));
                root->AddNode(littleGuy->GetTransformation());
                spermList->push_back(littleGuy);
            }
        spermMarked = new Spermatozoa(so);
        spermMarked->LoadTexture("SpermatozoaMarked.tga");
        spermMarked->Kill();
        spermMarked->Mark();
     }

    void Deinitialize() {
      list<Spermatozoa*>::iterator itr;
      for( itr = spermList->begin(); itr!=spermList->end(); itr++)
	root->RemoveNode( (*itr)->GetTransformation() );

      SimulationState::Deinitialize();
    }

    void Process(ProcessEventArg arg) {
        float delta = arg.approx / 1000.0;
        // Update all spermatozoas
        list<Spermatozoa*>::iterator itr;
        for( itr = spermList->begin(); itr!=spermList->end(); itr++) {
             (*itr)->Update(delta);
        }

        if (!changeState)
            CheckCollision(delta);

        SimulationState::Process(arg);
    };

    void CheckCollision(const float delta) {
        static Vector<3,float> oldPos = needleHandler->GetPosition();
        Vector<3,float> needlePos = needleHandler->GetPosition();
        float velocity = (oldPos - needlePos).GetLength();
        oldPos = needlePos;

        const float needleThickness = 0.5f;
        Spermatozoa* chosenOne = NULL;
        list<Spermatozoa*>::iterator itr;
        for( itr = spermList->begin(); itr!=spermList->end(); itr++ ){
            Vector<3,float> pos = (*itr)->GetPosition();
            Vector<3,float> dist = pos - needlePos;

            if( dist.GetLength() > 0 && dist.GetLength() < needleThickness ){
                // Collision detected - move the spermatozoa
                float moveDist = needleThickness-dist.GetLength();
                Vector<3,float> newPos = pos + Vector<3,float>(dist.GetNormalize() * moveDist);
                newPos[2] = -0.1;
                (*itr)->SetPosition(newPos);
                if( velocity > 0.36 && (*itr)->IsAlive() && (*itr)->IsHealthy() ){
                    // Spermatozoa killed - check if it was a successful kill
                    chosenOne = (*itr);
                    chosenOne->Kill();
                    spermMarked->SetPosition((*itr)->GetPosition());
                    break;
                }
            }

            if(  pos[0] > needlePos[0] && fabs(dist[1]) < needleThickness ){
                // Collision detected - move the spermatozoa
                Vector<3,float> newPos = pos;
                if (dist[1] > 0)
                    newPos[1] = needlePos[1] + needleThickness;
                else
                    newPos[1] = needlePos[1] - needleThickness;
                (*itr)->SetPosition(newPos);
            }
        }

        if( chosenOne != NULL ){
            spermList->remove(chosenOne);
            root->RemoveNode( chosenOne->GetTransformation() );
            spermList->push_back(spermMarked);
            root->AddNode(spermMarked->GetTransformation());
            changeState = true;
        }
    }
};

#endif
