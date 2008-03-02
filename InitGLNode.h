#ifndef _INIT_GL_NODE_
#define _INIT_GL_NODE_

#include <Meta/OpenGL.h>
#include <Renderers/IRenderNode.h>
#include <Renderers/IRenderingView.h>

using namespace OpenEngine::Renderers;

class InitGLNode : public IRenderNode {
public:
  InitGLNode() {}
  void Apply(IRenderingView* rv) {
    // Clear the OpenGL framebuffer.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
    
    // Light settings.
    GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
    GLfloat lm_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    
    // Black Background
    glClearColor(0.0f,0.0f,0.0f,0.5f);
    // Depth Buffer Setup
    glClearDepth(1.0f);                                
    // The Type Of Depth Testing (Less Or Equal)
    glDepthFunc(GL_LEQUAL);							   
    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);						   
    // Select Smooth Shading
    glShadeModel(GL_SMOOTH);						   
    // Set Perspective Calculations To Most Accurate
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
    // Enable backface culling, only faces facing towards the view is rendered.
    //glEnable(GL_CULL_FACE);
    // Enable color material, including face colors when blending. 
    glEnable(GL_COLOR_MATERIAL);
    
    // To enable blending just enable the following three lines: 
    glEnable(GL_BLEND);
    // Set The Blending Function For Translucency
    glBlendFunc(GL_SRC_COLOR, GL_ONE);
    glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
    //glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off

    VisitSubNodes(*rv);
  }
};

#endif
