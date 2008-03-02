// Inseminator Simulator.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>

#include <Core/GameEngine.h>

#include "Meta/GLUT.h"//TEMP ONLY FOR DRAWING WIRED SPHERE IN MediPhysic
#include "Factory.h"

using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;

int main( int argc, char** argv ) {
    glutInit(&argc, argv);//TEMP ONLY FOR DRAWING WIRED SPHERE IN MediPhysic

    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    logger.info << "========== Running OpenEngine Physic ========" << logger.end;

    // Start the engine.
    IGameEngine& engine = GameEngine::Instance();
    engine.SetTickTime(20); // for the MediPhysic to work properly
    engine.Start(new Factory);

    // Return when the engine stops.
    return EXIT_SUCCESS;
}
