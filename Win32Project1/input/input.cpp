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

void Input::updateCamera(Camera* camera) {
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

void Input::update(RenderManager* renderMgr) {
	if (extra[0])
		renderMgr->lightDir.x -= L_DISTANCE;
	if (extra[1])
		renderMgr->lightDir.x += L_DISTANCE;
	if (extra[2])
		renderMgr->lightDir.z -= L_DISTANCE;
	if (extra[3])
		renderMgr->lightDir.z += L_DISTANCE;
}

