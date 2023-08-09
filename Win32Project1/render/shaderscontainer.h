/*
 * shaderscontainer.h
 *
 *  Created on: 2017-9-14
 *      Author: a
 */

#ifndef SHADERSCONTAINER_H_
#define SHADERSCONTAINER_H_

#include "../shader/shadermanager.h"
#include "../util/util.h"

#define WORKGROUPE_SIZE 1
#define COMP_GROUPE_SIZE 4
#define MAX_DISPATCH 8192
#define MAX_OIT_LAYER 4

void SetupShaders(ShaderManager* shaders, const ConfigArg* cfgs);
void ClearAllBindingTex(ShaderManager* shaders);

#endif /* SHADERSCONTAINER_H_ */
