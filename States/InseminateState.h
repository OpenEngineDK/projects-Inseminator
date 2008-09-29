#ifndef _INSEMINATE_STATE_H_
#define _INSEMINATE_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Utils/Timer.h>
#include <Utils/Billboard.h>

using OpenEngine::Utils::Timer;

const float TIME_OUT = 1500;

class InseminateState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    float time;
    bool failed;
    Timer timer;
public:
 InseminateState(string nextState, MediPhysic* physic, StateObjects& so)
     : SimulationState(nextState, so) {
        this->physic = physic;
        time = 0;
        failed = false;
    }
    ~InseminateState() {}

    void Initialize(){
        root = so.GetSceneNode();

        // insert the physics node into and initialize timer
        root->AddNode(physic);
        // initialization of physics is done by the TurnTheEggState
        timer.Start();
        // physic node must be in the tree before initializing
        // the needle, or the transparency will f...

        SimulationState::Initialize(); //this also initializes the needle

        needleHandler->EnableSuck(true);

        // Load texture
        failedTexture = Billboard::Create("Failed2.tga", 128, 64, 0.07);
        failedTexture->SetPosition(Vector<3,float>(3,0,-5));

        failed = false;
    }

    void Deinitialize() {
        root->RemoveNode(physic);
        physic->Handle(DeinitializeEventArg());

        if(physic->transNode!=NULL)
            root->RemoveNode(physic->transNode);

        SimulationState::Deinitialize();
    }

    void Process(ProcessEventArg arg) {
        float delta = arg.approx / 1000.0;

        // Check if we the injecting went well
        Spermatozoa* spermatozoa = needleHandler->GetReleasedSpermatozoa();
        if (spermatozoa != NULL && !changeState && !failed) {
            if (physic->Inject(spermatozoa->GetTransformation())) { // went well, sucess
                changeState = true;
            } else { // wrong, reset the simulation
                root->AddNode(failedTexture);
                failed = true;
            }
        }

        if (failed) time += delta;
        if (time > TIME_OUT) {
            root->RemoveNode(spermatozoa->GetTransformation());
            needleHandler->SetSpermatozoa(spermatozoa);
            needleHandler->Reset();
            root->RemoveNode(failedTexture);
            time = 0;
            failed = false;
        }

        // process physics time dependend
        const unsigned int tick = 50;
        unsigned int t = timer.GetElapsedIntervalsAndReset(tick*1000);
        while (t--) physic->Handle(ProcessEventArg(arg.start,tick*1000));

        SimulationState::Process(arg);
    }
};

#endif
