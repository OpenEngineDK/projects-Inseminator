# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "Inseminator")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
     main.cpp
     Factory.cpp
     States/BaseState.cpp
     States/SimulationState.cpp
     States/SelectState.cpp
     States/HUDisplay.cpp
     States/HUDMovie.cpp
     States/Background.cpp 
     States/Spermatozoa.cpp
)

# Include needed to use SDL under Mac OS X
IF(APPLE)
  SET(PROJECT_SOURCES ${PROJECT_SOURCES} ${SDL_MAIN_FOR_MAC})
ENDIF(APPLE)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
  OpenEngine_Core
  OpenEngine_Devices
  OpenEngine_Display
  OpenEngine_Geometry
  OpenEngine_Logging
  OpenEngine_Math
  OpenEngine_Resources
  OpenEngine_Scene
  # Extension dependencies
  Extensions_OpenGLRenderer
  Extensions_HUD
  Extensions_SDLImage
  Extensions_OBJResource
  Extensions_SDL
  Extensions_StateManager
  Extensions_MediPhysics
  Extensions_FFMPEGResource
 )
