#ifndef _SELECT_STATE_H_
#define _SELECT_STATE_H_

#include "SimulationState.h"

class SelectState : public SimulationState {
private:
    list<Spermatozoa*>* spermList;
    Spermatozoa* chosenOne;
    Spermatozoa* normalSperm;
    TransformationNode* failedTexture;
    bool eval, failedTextIsVisible;
public:
    SelectState(string nextState, StateObjects& so);
    ~SelectState();

    void Initialize();
    void Deinitialize();
    void Process(ProcessEventArg arg);

    void CheckCollision();
    void SetSpermatozoaList(list<Spermatozoa*>* s);
};

#endif
