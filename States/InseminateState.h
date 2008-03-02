#ifndef _INSEMINATE_STATE_H_
#define _INSEMINATE_STATE_H_

#include "MediPhysic.h"
#include "SimulationState.h"

#include <Utils/Billboard.h>

const float TIME_OUT = 1500;

class InseminateState : public SimulationState {
private:
    MediPhysic* physic;
    TransformationNode* failedTexture;
    float time;
    bool failed;
public:
 InseminateState(string nextState, MediPhysic* physic) : SimulationState(nextState) {
        this->physic = physic;
        time = 0;
        failed = false;
    }
    ~InseminateState() {}

    void Initialize(){
        IRenderer* rendere = dynamic_cast<IRenderer*>
	  (IGameEngine::Instance().Lookup(typeid(IRenderer)));
        root = rendere->GetSceneRoot();      

        root->AddNode(physic);
        IGameEngine::Instance().AddModule(*physic, IGameEngine::TICK_DEPENDENT);
	/* physic node must be in the tree before initializing
	   the needle, or the transparency will f... */

        SimulationState::Initialize(); //this also initializes the needle

        needleHandler->EnableSuck(true);

        // Load texture
        failedTexture = Billboard::Create("Failed2.tga", 128, 64, 0.07);
        failedTexture->SetPosition(Vector<3,float>(3,0,-5));

        failed = false;
    }

    void Deinitialize() {
        root->RemoveNode(physic);
        physic->Deinitialize();
        IGameEngine::Instance().RemoveModule(*physic);

        if(physic->transNode!=NULL)
            root->RemoveNode(physic->transNode);

	SimulationState::Deinitialize();
    }

    bool IsTypeOf(const std::type_info& inf) { return typeid(InseminateState) == inf; }

    void Process(const float delta, const float percent) {
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

        SimulationState::Process(delta,percent);
    }
};

#endif
