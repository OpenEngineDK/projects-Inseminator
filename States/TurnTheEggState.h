#ifndef _TURN_THE_EGG_STATE_H_
#define _TURN_THE_EGG_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Core/EngineEvents.h>

using namespace OpenEngine::Core;

class TurnTheEggState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    IEngine& engine;

public:
    TurnTheEggState(string nextState, MediPhysic* physic, IEngine& engine
                    , StateObjects& so)
        : SimulationState(nextState, so), engine(engine) {
        this->physic = physic;
    }
    ~TurnTheEggState() {}

    void Initialize() {
        root = so.GetSceneNode();

        root->AddNode(physic);
        physic->Handle(InitializeEventArg());

        engine.InitializeEvent().Attach(*physic);
        //@@@todo: should be IEngine::TICK_DEPENDENT
        engine.ProcessEvent().Attach(*physic);

        engine.DeinitializeEvent().Attach(*physic);
        // physic node must be in the tree before initializing
        // the needle, or the transparency will f...

        SimulationState::Initialize(); //this also initializes the needle
    }

    void Deinitialize() {
        root->RemoveNode(physic);
        engine.InitializeEvent().Detach(*physic);
        //@@@todo: should be IEngine::TICK_DEPENDENT
        engine.ProcessEvent().Detach(*physic);
        engine.DeinitializeEvent().Detach(*physic);

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
        
        static int c = 0;
        if (c++>10) {
            c = 0;
            //logger.info << "dir1: " << dir1[0] << " dir2: " << dir2[0] << logger.end;
	    //logger.info << "angle1: " << angle1 << ", angle2: " << angle2 << logger.end;
        } 
        if( angle1 < 0.6f && angle1 > 0.2f &&
            angle2 > -0.6f && angle2 < -0.2f &&
            (dir1[0] > 1.7f || dir1[0] < 1.0f) ) {
            //logger.info << "Egg is turned right" << logger.end;
            changeState = true;
        }
    }
};

#endif
