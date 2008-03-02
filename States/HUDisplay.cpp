// Program name and description
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "HUDisplay.h"
#include <Meta/OpenGL.h>
#include <Logging/Logger.h>

HUDisplay::HUDisplay() : textureId(0) {
    hasInitialized = false;
    fade = 1.0f;
}

HUDisplay::HUDisplay(int textureId) : textureId(textureId), fade(1.0f) {
    reverseTexture = false;
    hasInitialized = false;
    endRequested = false;
    blend = false;

    // the top and rightmost texture coords.
    maxX = 1.0;
    maxY = 1.0;
}

HUDisplay::~HUDisplay(){
}

void HUDisplay::Apply(IRenderingView* rende) {
    if(blend) { // used for Microscope view
        // Blend options
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);
        glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
    }
    else // used when rendering pictures and video
        glDisable(GL_BLEND);

    ApplyOrthoView();
    DrawInOrthoMode();
    ApplyProjectionView();
}

void HUDisplay::DrawInOrthoMode() {
  glBegin(GL_QUADS);
  glColor4f(1-fade, 1-fade, 1-fade, 1);
  if (reverseTexture) { // movies texture are reversed
    // Display the top left point of the 2D image
    glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, 0);
    // Display the bottom left point of the 2D image
    glTexCoord2f(0.0f, maxY);	glVertex2f(0, SCREEN_HEIGHT);
    // Display the bottom right point of the 2D image
    glTexCoord2f(maxX, maxY);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Display the top right point of the 2D image
    glTexCoord2f(maxX, 0.0f);	glVertex2f(SCREEN_WIDTH, 0);
  } else {
    // Display the top left point of the 2D image
    glTexCoord2f(0.0f, maxY);	glVertex2f(0, 0);
    // Display the bottom left point of the 2D image
    glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, SCREEN_HEIGHT);
    // Display the bottom right point of the 2D image
    glTexCoord2f(maxX, 0.0f);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Display the top right point of the 2D image
    glTexCoord2f(maxX, maxY);	glVertex2f(SCREEN_WIDTH, 0);
  }
  glEnd();
}

void HUDisplay::ApplyOrthoView(){
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);

    /* Push on a new matrix so that we can just
       pop it off to go back to perspective mode */
    glPushMatrix();

    // Reset the current matrix to our identify matrix
    glLoadIdentity();
    
    /* Pass in our 2D ortho screen coordinates.like
       so (left, right, bottom, top).  The last
       2 parameters are the near and far planes. */
    glOrtho( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1 );	
    
    // Switch to model view so that we can render the scope image
    glMatrixMode(GL_MODELVIEW);

    // Initialize the current model view matrix with the identity matrix
    glLoadIdentity();					
  
    glEnable(GL_TEXTURE_2D);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glBindTexture(GL_TEXTURE_2D, textureId);
}

void HUDisplay::Process(const float delta, const float percent) {
    if (hasInitialized && endRequested)
    fade += 0.001 * delta; //fade down
    else if (!hasInitialized)
        fade -= 0.001 * delta; // fade up
    if (fade <= 0.0f) { // if fully faded up
        fade = 0.0f;
        hasInitialized = true;
    }
}

void HUDisplay::FadeDown() {
    this->endRequested = true;
}

void HUDisplay::Blend(bool blend) {
    this->blend = blend;
}

bool HUDisplay::Ended() {
    return (hasInitialized && (fade >= 1.0f));
}

void HUDisplay::ApplyProjectionView() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Enter into our projection matrix mode
    glMatrixMode( GL_PROJECTION );
    
    /* Pop off the last matrix pushed on when in
       projection mode(Get rid of ortho mode) */
    glPopMatrix();

    // Go back to our model view matrix like normal
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
}
