#include "SelectState.h"

#include "Utils/Billboard.h"

SelectState::SelectState(string nextState, StateObjects& so)
  : SimulationState(nextState, so) { 
        eval = false;
}

SelectState::~SelectState() {}

void SelectState::Initialize() {
    SimulationState::Initialize();
    needleHandler->EnableSuck(true);

    list<Spermatozoa*>::iterator itr;
    for (itr = spermList->begin(); itr!=spermList->end(); itr++)
        root->AddNode( (*itr)->GetTransformation() );

    normalSperm = new Spermatozoa(so);
    normalSperm->LoadTexture("SpermatozoaNormal-withalpha.tga");
    
    failedTexture = Billboard::
        Create("Failed1-pustud-withalpha.tga", 128, 64, 0.07);
    so.GetTextureLoader().Load(*failedTexture);
    failedTexture->SetPosition(Vector<3,float>(3,0,-5));
}

void SelectState::Deinitialize() {
    list<Spermatozoa*>::iterator itr;
    for( itr = spermList->begin(); itr!=spermList->end(); itr++) {
        root->RemoveNode( (*itr)->GetTransformation() );
    }

    SimulationState::Deinitialize();
}

void SelectState::Process(ProcessEventArg arg) {
    float delta = arg.approx / 1000.0;

    SimulationState::Process(arg);

    // stops the rotation and movement when done
    if (changeState) return;

    // Update all spermatozoas
    list<Spermatozoa*>::iterator itr;
    for( itr = spermList->begin(); itr!=spermList->end(); itr++)
        (*itr)->Update(delta);
	
    chosenOne = NULL;
    if( needleHandler->IsSucking() ){
        Vector<3,float> needlePos = needleHandler->GetPosition();
	float shortestDist = 100.0f;
	// Find the closes spermatozoa 
	list<Spermatozoa*>::iterator itr;
	for( itr = spermList->begin(); itr!=spermList->end(); itr++) {
	    Vector<3,float> dist = (*itr)->GetPosition() - needlePos;
	    if( dist.GetLength() < shortestDist ){
	        shortestDist = dist.GetLength();
		chosenOne = *itr;
	    }
	}
    }
    
    // Try to suck up the closes spermatozoa.
    if( chosenOne != NULL ) needleHandler->Suck(chosenOne);
    
    // Check if the Needle has sucked up the marked spermatozoa.
    if( needleHandler->GetSpermatozoa() != NULL && !eval ){
        eval = true;
	if( needleHandler->SuckedUpRight() &&
        needleHandler->GetSpermatozoa()->IsMarked() ){
	    Spermatozoa* suckedUp = needleHandler->GetSpermatozoa();
	    //logger.info << "Sucked up successfully" << logger.end;
	    
	    // The right spermatozoa is sucked up the right way - remove marked texture
	    normalSperm->SetPosition(Vector<3,float>(0,0,0));
	    needleHandler->SetSpermatozoa(normalSperm);
	    spermList->push_back(normalSperm);
	    spermList->remove(suckedUp);

	    // offset the spermztozoa inside the needle
	    needleHandler->GetSpermatozoa()->GetTransformation()->Move(2.0f, 0,0);

	    changeState = true;
	    return;
	} else {
	    //logger.info << "You sucked up the WRONG one!" << logger.end;
	    root->AddNode(failedTexture);
	}
    }
    if( needleHandler->GetSpermatozoa() == NULL ){
        eval = false;
        root->RemoveNode(failedTexture);
    }
    CheckCollision();
}
    
void SelectState::CheckCollision() {
    Vector<3,float> needlePos = needleHandler->GetPosition();
    const float needleThickness = 0.5f;
    
    list<Spermatozoa*>::iterator itr;
    for( itr = spermList->begin(); itr!=spermList->end(); itr++ ) {
        if( *itr == chosenOne || needleHandler->GetSpermatozoa() == *itr ) return; // no collision on the chosen one
	Vector<3,float> pos = (*itr)->GetPosition();
	Vector<3,float> dist = pos - needlePos;
	
	if( dist.GetLength() > 0 && dist.GetLength() < needleThickness ){
	    // Collision detected - move the spermatozoa
	    float moveDist = needleThickness-dist.GetLength();
	    Vector<3,float> newPos = pos + Vector<3,float>(dist.GetNormalize() * moveDist);
	    newPos[2] = 0;
	    (*itr)->SetPosition(newPos);
	}

	if(  pos[0] > needlePos[0] && fabs(dist[1]) < needleThickness ){
	    // Collision detected - move the spermatozoa
	    Vector<3,float> newPos = pos;
	    
	    if( dist[1] > 0 )
	        newPos[1] = needlePos[1] + needleThickness;
	    else
	        newPos[1] = needlePos[1] - needleThickness;

	    (*itr)->SetPosition(newPos);
	}
    }
}        

void SelectState::SetSpermatozoaList(list<Spermatozoa*>* s) {
    spermList = s;
}
