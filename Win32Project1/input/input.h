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

#define KEY_A 65
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_G 71
#define KEY_Q 81
#define KEY_R 82
#define KEY_S 83
#define KEY_V 86
#define KEY_W 87
#define KEY_Z 90

#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57

class Input {
private:
	bool* boards;
	int controlId;
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
	bool* getBoards() { return boards; }
	void setControl(int cid) { controlId = cid; }
	int getControl() { return controlId; }
};

#endif /* INPUT_H_ */
