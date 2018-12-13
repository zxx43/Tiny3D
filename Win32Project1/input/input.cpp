#include "input.h"
#include <string.h>
#include "../constants/constants.h"

Input::Input() {
	turn=new bool[4];
	move=new bool[6];
	memset(turn,0,4*sizeof(bool));
	memset(move,0,6*sizeof(bool));
	extra = new bool[4];
	memset(extra, 0, 4 * sizeof(bool));
}

Input::~Input() {
	delete[] turn; turn=NULL;
	delete[] move; move=NULL;
	delete[] extra; extra = NULL;
}

void Input::keyDown(int key) {
	if(key=='W')
		move[0]=true;
	if(key=='S')
		move[1]=true;
	if(key=='A')
		move[2]=true;
	if(key=='D')
		move[3]=true;
	if(key==KEY_SPACE)
		move[4]=true;
	if(key=='Z')
		move[5]=true;
	if(key==KEY_UP)
		turn[0]=true;
	if(key==KEY_DOWN)
		turn[1]=true;
	if(key==KEY_LEFT)
		turn[2]=true;
	if(key==KEY_RIGHT)
		turn[3]=true;

	if (key == KEY_LEFT_EX)
		extra[0] = true;
	if (key == KEY_RIGHT_EX)
		extra[1] = true;
	if (key == KEY_UP_EX)
		extra[2] = true;
	if (key == KEY_DOWN_EX)
		extra[3] = true;
}

void Input::keyUp(int key) {
	if(key=='W')
		move[0]=false;
	if(key=='S')
		move[1]=false;
	if(key=='A')
		move[2]=false;
	if(key=='D')
		move[3]=false;
	if(key==KEY_SPACE)
		move[4]=false;
	if(key=='Z')
		move[5]=false;
	if(key==KEY_UP)
		turn[0]=false;
	if(key==KEY_DOWN)
		turn[1]=false;
	if(key==KEY_LEFT)
		turn[2]=false;
	if(key==KEY_RIGHT)
		turn[3]=false;

	if (key == KEY_LEFT_EX)
		extra[0] = false;
	if (key == KEY_RIGHT_EX)
		extra[1] = false;
	if (key == KEY_UP_EX)
		extra[2] = false;
	if (key == KEY_DOWN_EX)
		extra[3] = false;
}

void Input::updateCameraByKey(Camera* camera) {
	if(move[0])
		camera->move(MNEAR,D_DISTANCE);
	if(move[1])
		camera->move(MFAR,D_DISTANCE);
	if(move[2])
		camera->move(LEFT,D_DISTANCE);
	if(move[3])
		camera->move(RIGHT,D_DISTANCE);
	if(move[4])
		camera->move(UP,D_DISTANCE);
	if(move[5])
		camera->move(DOWN,D_DISTANCE);

	if(turn[0])
		camera->turnY(UP);
	if(turn[1])
		camera->turnY(DOWN);
	if(turn[2])
		camera->turnX(LEFT);
	if(turn[3])
		camera->turnX(RIGHT);
}

void Input::updateExtra(RenderManager* renderMgr) {
	bool triggered = false;
	if (extra[0]) {
		renderMgr->lightDir.x -= L_DISTANCE;
		triggered = true;
	}
	if (extra[1]) {
		renderMgr->lightDir.x += L_DISTANCE;
		triggered = true;
	}
	if (extra[2]) {
		renderMgr->lightDir.z -= L_DISTANCE;
		triggered = true;
	}
	if (extra[3]) {
		renderMgr->lightDir.z += L_DISTANCE;
		triggered = true;
	}
	if (triggered)
		renderMgr->lightDir.Normalize();
}

void Input::updateCameraByMouse(Camera* camera, const float mouseX,
		const float mouseY, const float centerX, const float centerY) {
	if (mouseX == centerX && mouseY == centerY) return;
	const static float cosdr = cos(A2R);
	const static float sindr = sin(A2R);
	const static float mag = 0.5 * 0.001f * R2A;
	float dxr = (centerX - mouseX) * mag;
	float dyr = (centerY - mouseY) * mag;
	camera->turnDX(dxr*cosdr);
	camera->turnDX(-dyr*sindr);
	camera->turnDY(dxr*sindr);
	camera->turnDY(dyr*cosdr);
	camera->updateMoveable(TRANS_ROTATE_X | TRANS_ROTATE_Y);
}

void Input::moveCamera(Camera* camera, int direction) {
	camera->move(direction, D_DISTANCE * 10.0);
}
