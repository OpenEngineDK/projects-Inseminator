# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "Inseminator")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
     main.cpp
     Factory.cpp
     States/HUDisplay.cpp
     States/HUDMovie.cpp
     States/Background.cpp 
     States/Spermatozoa.cpp
)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

# Include needed to use SDL under Mac OS X
IF(APPLE)
  SET(PROJECT_SOURCES ${PROJECT_SOURCES} ../../conf/mac/SDLMain.m)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}
 		       OpenEngine_Core
		       OpenEngine_Display
		       OpenEngine_Resources
		       OpenEngine_Scene
		       OpenEngine_Logging
		       OpenEngine_Devices
		       OpenEngine_Scene
		       OpenEngine_Utils
		       OpenEngine_Geometry
		       OpenEngine_Renderers_OpenGL
 		       ${OPENGL_LIBRARY}
		       ${GLEW_LIBRARY}
  # Extension dependencies
  Extensions_StateManager
  Extensions_MediPhysics
  Extensions_MovieResource
  Extensions_GLScreenshot
 )
