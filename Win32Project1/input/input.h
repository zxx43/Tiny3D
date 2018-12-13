/*
 * input.h
 *
 *  Created on: 2017-4-8
 *      Author: a
 */

#ifndef INPUT_H_
#define INPUT_H_

#include "../render/renderManager.h"

#define KEY_ESC 27
#define KEY_SPACE 32
#define KEY_LEFT 37
#define KEY_UP 38
#define KEY_RIGHT 39
#define KEY_DOWN 40
#define KEY_LEFT_EX 100
#define KEY_RIGHT_EX 102
#define KEY_UP_EX 104
#define KEY_DOWN_EX 98

class Input {
private:
	bool* turn;
	bool* move;
	bool* extra;
public:
	Input();
	~Input();
	void keyDown(int key);
	void keyUp(int key);
	void updateCameraByKey(Camera* camera);
	void updateExtra(RenderManager* renderMgr);
	void updateCameraByMouse(Camera* camera, const float mouseX, 
		const float mouseY, const float centerX, const float centerY);
	void moveCamera(Camera* camera, int direction);
};

#endif /* INPUT_H_ */
