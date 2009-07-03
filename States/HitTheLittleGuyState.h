#ifndef _HIT_THE_LITTLE_GUY_STATE_H_
#define _HIT_THE_LITTLE_GUY_STATE_H_

#include "NeedleHandler.h"
#include "Spermatozoa.h"
#include "SimulationState.h"

class HitTheLittleGuyState : public SimulationState {
private:
    list<Spermatozoa*>* spermList;
    Spermatozoa* spermMarked;

public:
 HitTheLittleGuyState(string nextState, StateObjects& so)
     : SimulationState(nextState, so) {
    }
    ~HitTheLittleGuyState() {}

    void SetSpermatozoaList(list<Spermatozoa*>* spermList) {
        this->spermList = spermList;
    }

    void Initialize() {
        SimulationState::Initialize();

        list<Spermatozoa*>::iterator itr;
        for (itr = spermList->begin(); itr!=spermList->end(); itr++)
            root->AddNode( (*itr)->GetTransformation() );

        spermMarked = new Spermatozoa(so);
        spermMarked->LoadTexture("SpermatozoaMarked-withalpha.png");
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
                if( velocity > 0.37 && (*itr)->IsAlive() && (*itr)->IsHealthy() ){
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
