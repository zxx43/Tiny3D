#include "input.h"
#include <string.h>
#include <stdlib.h>
#include "../constants/constants.h"

Input::Input() {
	boards = (bool*)malloc(512 * sizeof(bool));
	memset(boards, 0, 512 * sizeof(bool));
	setControl(-1);
}

Input::~Input() {
	free(boards);
}

void Input::keyDown(int key) {
	boards[key] = true;
}

void Input::keyUp(int key) {
	boards[key] = false;
}

void Input::updateCameraByKey(Camera* camera) {
	if (controlId >= 0) return;

	if (boards[KEY_W])
		camera->move(MNEAR, camera->velocity);
	if (boards[KEY_S])
		camera->move(MFAR, camera->velocity);
	if (boards[KEY_A])
		camera->move(LEFT, camera->velocity);
	if (boards[KEY_D])
		camera->move(RIGHT, camera->velocity);
	if (boards[KEY_SPACE])
		camera->move(UP, camera->velocity);
	if (boards[KEY_Z])
		camera->move(DOWN, camera->velocity);

	if(boards[KEY_UP])
		camera->turnY(UP);
	if(boards[KEY_DOWN])
		camera->turnY(DOWN);
	if(boards[KEY_LEFT])
		camera->turnX(LEFT);
	if(boards[KEY_RIGHT])
		camera->turnX(RIGHT);
}

void Input::updateExtra(RenderManager* renderMgr) {
	bool triggered = false;
	if (boards[KEY_LEFT_EX]) {
		renderMgr->lightDir.x -= L_DISTANCE;
		triggered = true;
	}
	if (boards[KEY_RIGHT_EX]) {
		renderMgr->lightDir.x += L_DISTANCE;
		triggered = true;
	}
	if (boards[KEY_DOWN_EX]) {
		renderMgr->lightDir.z -= L_DISTANCE;
		triggered = true;
	}
	if (boards[KEY_UP_EX]) {
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
