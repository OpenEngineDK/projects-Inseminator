#ifndef _TURN_THE_EGG_STATE_H_
#define _TURN_THE_EGG_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

class TurnTheEggState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;

public:
    TurnTheEggState(string nextState, MediPhysic* physic) : SimulationState(nextState) {
        this->physic = physic;
    }
    ~TurnTheEggState() {}

    void Initialize() {
        IRenderer* rendere = dynamic_cast<IRenderer*>
	  (IGameEngine::Instance().Lookup(typeid(IRenderer)));
        root = rendere->GetSceneRoot();      

        root->AddNode(physic);
        physic->Initialize();
        IGameEngine::Instance().AddModule(*physic, IGameEngine::TICK_DEPENDENT);
	/* physic node must be in the tree before initializing
	   the needle, or the transparency will f... */

        SimulationState::Initialize(); //this also initializes the needle
    }

    void Deinitialize() {
        root->RemoveNode(physic);
        IGameEngine::Instance().RemoveModule(*physic);
	SimulationState::Deinitialize();
    }

    bool IsTypeOf(const std::type_info& inf) { return typeid(TurnTheEggState) == inf; }

    void Process(const float delta, const float percent) {
        SimulationState::Process(delta,percent);

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
