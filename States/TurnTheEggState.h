#ifndef _TURN_THE_EGG_STATE_H_
#define _TURN_THE_EGG_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Core/EngineEvents.h>
#include <Scene/BlendingNode.h>
#include <Utils/Billboard.h>
#include <Utils/Timer.h>

using namespace OpenEngine::Core;
using OpenEngine::Utils::Timer;

class TurnTheEggState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    TransformationNode* infoTexture;
    IEngine& engine;
    Timer timer;
    BlendingNode* blendingNode;
    bool infoTextIsVisible;
public:
    TurnTheEggState(string nextState, MediPhysic* physic, IEngine& engine
                    , StateObjects& so)
        : SimulationState(nextState, so), engine(engine) {
        this->physic = physic;

        blendingNode = new BlendingNode();
        blendingNode->SetSource(BlendingNode::SRC_COLOR);
        blendingNode->SetDestination(BlendingNode::ONE);
        blendingNode->SetEquation(BlendingNode::REVERSE_SUBTRACT);
    }
    ~TurnTheEggState() {}

    void Initialize() {
        SimulationState::Initialize(); //this also initializes the needle

        // information image
        infoTexture = Billboard::
            Create("TurnTheEggInfo-withalpha.png", 128, 64, 0.05);
        infoTexture->SetPosition(Vector<3,float>(3,-3,-5));
        infoTextIsVisible = false;
        so.GetTextureLoader().Load(*infoTexture);

        // insert the physics node into and initialize timer
        root->AddNode(blendingNode);
        blendingNode->AddNode(physic);
        physic->Handle(Core::InitializeEventArg());
        timer.Start();

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
        // deinitialization of physics is done by the InseminationState

		if (infoTextIsVisible)
            root->RemoveNode(infoTexture);

        SimulationState::Deinitialize();
    }

    void Process(Core::ProcessEventArg arg) {
        SimulationState::Process(arg);

        Vector<3,float> dir1 = physic->GetOrientationVector(22);
        Vector<3,float> dir2 = physic->GetOrientationVector(216);
        dir1[2] = 0;
        dir2[2] = 0;

        Vector<3,float> horiz(1,0,0);
        float angle1 = horiz * dir1.GetNormalize();
        float angle2 = horiz * dir2.GetNormalize();
        
        /*
        static int c = 0;
        if (c++>10) {
            c = 0;
            logger.info << "dir1: " << dir1[0]
                        << " dir2: " << dir2[0] << logger.end;
            logger.info << "angle1: " << angle1 
                        << ", angle2: " << angle2 << logger.end;
        } 
        */

        if( angle1 < 0.6f && angle1 > 0.2f &&
            angle2 > -0.6f && angle2 < -0.2f &&
            (dir1[0] > 1.7f || dir1[0] < 1.0f) ) {
            //logger.info << "Egg was turned correctly" << logger.end;
            changeState = true;
        }

        // process physics time dependend
        const unsigned int tick = 50;
        unsigned int t = timer.GetElapsedIntervalsAndReset(tick*1000);
        while (t--) physic->Handle(Core::ProcessEventArg(arg.start,tick*1000));

        if (stateClock > 10000 && !infoTextIsVisible) {
            root->AddNode(infoTexture);
            infoTextIsVisible = true;
        }
    }
};

#endif
