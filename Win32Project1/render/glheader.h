/*
 * glheader.h
 *
 *  Created on: 2017-4-7
 *      Author: a
 */

#ifndef GLHEADER_H_
#define GLHEADER_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#define CULL_NONE 0
#define CULL_BACK 1
#define CULL_FRONT 2
#define CULL_ALL 3

#define LESS 0
#define LEQUAL 1
#define GREATER 2
#define GEQUAL 3
#define ALWAYS 4

#define WRAP_REPEAT GL_REPEAT
#define WRAP_MIRROR GL_MIRRORED_REPEAT
#define WRAP_CLAMP_TO_EDGE GL_CLAMP_TO_EDGE
#define WRAP_CLAMP_TO_BORDER GL_CLAMP_TO_BORDER

#endif /* GLHEADER_H_ */
