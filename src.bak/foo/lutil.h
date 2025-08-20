
#ifndef LUTIL_H
#define LUTIL_H

#include "lopengl.h"

/* screen constants */
extern int g_screen_width;
extern int g_screen_height;
extern int g_screen_fps;


int init_gl (void);
/* Pre Condition:
 *  -A valid OpenGL context
 * Post Condition:
 *  -Initializes matrices and clear color
 *  -Reports to console if there was an OpenGL error
 *  -Returns false if there was an error in initialization
 * Side Effects:
 *  -Set viewport to the fill rendering area
 *  -Projectoin matrix is set to identity matrix
 *  -Modelview  matrix is set to identity matrix
 *  -Matrix mode is set to modelview
 *  -Clear color is set to black 
 *  -Texturing is enabled */

int load_media (void);
/* Pre Condition: 
 *  -A valid OpenGL context
 * Post Condition:
 *  -Loads media to use in the program
 *  -Reports to console if there was an error in loading the media
 *  -Returns 0 if the media loaded successfully */


void update (void);
/* Post Condition:
 *  -Does per frame logic */


void render (void);
/* Pre Condition:
 *  -A valid OpenGL context
 *  -Active modelview matrix
 * Post Condition:
 *  -Renders the scene
 * Side Effects:
 *  -Clears the color buffer
 *  -Swaps the front/back buffer */


void handle_keys (unsigned char key, int x, int y);
/* Post Condition:
 *  -handles keypresses */

void reshape (int width, int height);
/* Pre Condition:
 *  -A valid OpenGL context
 * Post Condition:
 *  -update program variables on window resize
 * Side Effects:
 *  -update g_screen_width and g_screen_height. */

#endif
/* end of file */
