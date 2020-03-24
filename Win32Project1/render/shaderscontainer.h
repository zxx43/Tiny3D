/*
 * shaderscontainer.h
 *
 *  Created on: 2017-9-14
 *      Author: a
 */

#ifndef SHADERSCONTAINER_H_
#define SHADERSCONTAINER_H_

#include "../shader/shadermanager.h"

#define WORKGROUPE_SIZE 1
#define COMP_GROUPE_SIZE 16
#define MAX_DISPATCH 2048

void SetupShaders(ShaderManager* shaders);

#endif /* SHADERSCONTAINER_H_ */
