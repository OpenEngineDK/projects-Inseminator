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

HUDisplay::HUDisplay(unsigned int width, unsigned int height) : textureId(0) {
    SCREEN_WIDTH = width; // = 1280;
    SCREEN_HEIGHT = height; // = 600;
    hasInitialized = false;
    fade = 1.0f;
}

HUDisplay::HUDisplay(ITextureResourcePtr texture) : fade(1.0f) {
    SCREEN_WIDTH = texture->GetWidth(); // = 1280;
    SCREEN_HEIGHT = texture->GetHeight(); // = 600;
    textureId = texture->GetID();
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
    /*
    if (blend) { // used for Microscope view
        // Blend options
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);
        glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
    }
    else // used when rendering pictures and video
        glDisable(GL_BLEND);
*/
    CHECK_FOR_GL_ERROR();
    GLboolean lighting = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);

    GLboolean depthtest = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    CHECK_FOR_GL_ERROR();
    ApplyOrthoView();
    CHECK_FOR_GL_ERROR();
    DrawInOrthoMode(); //fading
    CHECK_FOR_GL_ERROR();
    ApplyProjectionView();
    CHECK_FOR_GL_ERROR();

    if (lighting) glEnable(GL_LIGHTING);
    if (depthtest) glEnable(GL_DEPTH_TEST);

    //glEnable(GL_COLOR_MATERIAL);
    CHECK_FOR_GL_ERROR();
}

void HUDisplay::DrawInOrthoMode() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);

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
    CHECK_FOR_GL_ERROR();
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
    CHECK_FOR_GL_ERROR();
    glOrtho( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1 );
    CHECK_FOR_GL_ERROR();
    
    // Switch to model view so that we can render the scope image
    glMatrixMode(GL_MODELVIEW);

    // Initialize the current model view matrix with the identity matrix
    glLoadIdentity();
}

void HUDisplay::Process(ProcessEventArg arg) {
    float delta = arg.approx / 1000.0;

    float blendFactor = 0.001;
    if (hasInitialized && endRequested)
        fade += blendFactor * delta; //fade down
    else if (!hasInitialized)
        fade -= blendFactor * delta; // fade up
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
    // Enter into our projection matrix mode
    glMatrixMode( GL_PROJECTION );
    
    /* Pop off the last matrix pushed on when in
       projection mode(Get rid of ortho mode) */
    glPopMatrix();

    // Go back to our model view matrix like normal
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
}
