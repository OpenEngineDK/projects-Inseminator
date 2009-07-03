#ifndef _INSEMINATE_STATE_H_
#define _INSEMINATE_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Utils/Timer.h>
#include <Utils/Billboard.h>

using OpenEngine::Utils::Timer;

const float TIME_OUT = 4000;

class InseminateState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    float time;
    bool failed;
    Timer timer;
    BlendingNode* blendingNode;
public:
 InseminateState(string nextState, MediPhysic* physic, StateObjects& so)
     : SimulationState(nextState, so) {
        this->physic = physic;
        time = 0;
        failed = false;

        blendingNode = new BlendingNode();
        blendingNode->SetSource(BlendingNode::SRC_COLOR);
        blendingNode->SetDestination(BlendingNode::ONE);
        blendingNode->SetEquation(BlendingNode::REVERSE_SUBTRACT);
    }
    ~InseminateState() {}

    void Initialize(){
        SimulationState::Initialize(); //this also initializes the needle

        // insert the physics node into and initialize timer
        root->AddNode(blendingNode);
        physic->Handle(InitializeEventArg());
        blendingNode->AddNode(physic);

        // @todo: hack for the spermatazoa to be rendered after the egg
        needleHandler->Deinitialize();
        needleHandler->Initialize();

        // initialization of physics is done by the TurnTheEggState
        timer.Start();
        // physic node must be in the tree before initializing
        // the needle, or the transparency will f...

        needleHandler->EnableSuck(true);

        // Load texture
        failedTexture = Billboard::
            Create("Failed2-indimidten-withalpha.png", 128, 64, 0.07);
        so.GetTextureLoader().Load(*failedTexture);
        failedTexture->SetPosition(Vector<3,float>(3,0,-5));

        failed = false;

        // if needle does not have a spermatazoa, add one
        if (needleHandler->GetSpermatozoa() == NULL) {
	        Spermatozoa* littleGuy = new Spermatozoa(so,true);
            littleGuy->LoadTexture("SpermatozoaNormal-withalpha.png");
            littleGuy->GetTransformation()->
                SetPosition(Vector<3,float>(0, 0, 0));
            needleHandler->SetSpermatozoa(littleGuy);

            // offset the spermztozoa inside the needle
            needleHandler->GetSpermatozoa()->
                GetTransformation()->Move(2.0f, 0,0);
        }
    }

    void Deinitialize() {
        blendingNode->RemoveNode(physic);
        root->RemoveNode(blendingNode);

        if (physic->transNode != NULL)
            root->RemoveNode(physic->transNode);
        physic->Handle(DeinitializeEventArg());

        SimulationState::Deinitialize();
    }

    void Process(ProcessEventArg arg) {
        float delta = arg.approx / 1000.0;

        // Check if we the injecting went well
        Spermatozoa* spermatozoa = needleHandler->GetReleasedSpermatozoa();
        if (spermatozoa != NULL && !changeState && !failed) {
            if (physic->Inject(spermatozoa->GetTransformation())) {
                // went well, sucess
                changeState = true;
            } else {
                // wrong, reset the simulation
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
