#ifndef _SELECT_STATE_H_
#define _SELECT_STATE_H_

#include "SimulationState.h"

class SelectState : public SimulationState {
private:
    list<Spermatozoa*>* spermList;
    Spermatozoa* chosenOne;
    Spermatozoa* normalSperm;
    TransformationNode* failedTexture;
    bool eval;
public:
    SelectState(string nextState);
    ~SelectState();

    void Initialize();
    void Deinitialize();
    bool IsTypeOf(const std::type_info& inf);
    void Process(const float delta, const float percent);
    void CheckCollision();
    void SetSpermatozoaList(list<Spermatozoa*>* s);
};

#endif
