// Inseminator Simulator.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>

#include "Factory.h"

#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>

#include <Core/Engine.h>

#include <Meta/GLUT.h> //TEMP ONLY FOR DRAWING WIRED SPHERE IN MediPhysic
#include <cstdlib>

using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;

int main( int argc, char** argv ) {
#ifndef __APPLE__
    //glutInit(&argc, argv);//TEMP ONLY FOR DRAWING WIRED SPHERE IN MediPhysic
#endif
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    logger.info << "========== Setting up Inseminator ========" << logger.end;
    std::string startState = "StartupPicture";
    if (argc == 2)
        startState = argv[1];
    logger.info << "Start state: " << startState << logger.end;

    // Start the engine.
    IEngine* engine = new Engine();
    //@todo engine.SetTickTime(20); // for the MediPhysic to work properly

    Factory* factory = new Factory();
    factory->SetupEngine(*engine,startState);

    logger.info << "=========== Running Inseminator ==========" << logger.end;
    engine->Start();

    //delete factory;
    //delete engine;

    // Return when the engine stops.
    return EXIT_SUCCESS;
}
