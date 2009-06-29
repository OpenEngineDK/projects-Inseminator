#ifndef _TURN_THE_EGG_STATE_H_
#define _TURN_THE_EGG_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Core/EngineEvents.h>
#include <Utils/Timer.h>

using namespace OpenEngine::Core;
using OpenEngine::Utils::Timer;

class TurnTheEggState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    IEngine& engine;
    Timer timer;

public:
    TurnTheEggState(string nextState, MediPhysic* physic, IEngine& engine
                    , StateObjects& so)
        : SimulationState(nextState, so), engine(engine) {
        this->physic = physic;
    }
    ~TurnTheEggState() {}

    void Initialize() {
        root = so.GetSceneNode();

        // insert the physics node into and initialize timer
        root->AddNode(physic);
        physic->Handle(InitializeEventArg());
        timer.Start();
        // physic node must be in the tree before initializing
        // the needle, or the transparency will f...

        if (needleHandler->GetSpermatozoa() == NULL) {
            logger.info << "no spermatozoa" << logger.end;
	        Spermatozoa* littleGuy = new Spermatozoa(so,true);
            littleGuy->LoadTexture("SpermatozoaNormal.tga");
            //littleGuy->GetTransformation()->
                //SetPosition(Vector<3,float>(-10, 0, 0));
            needleHandler->SetSpermatozoa(littleGuy);
        }

        SimulationState::Initialize(); //this also initializes the needle
    }

    void Deinitialize() {
        root->RemoveNode(physic);
        // deinitialization of physics is done by the InseminationState

        SimulationState::Deinitialize();
    }

    void Process(ProcessEventArg arg) {
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
        while (t--) physic->Handle(ProcessEventArg(arg.start,tick*1000));
    }
};

#endif
