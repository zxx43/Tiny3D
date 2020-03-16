#include "player.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include "../input/input.h"
#include "../camera/camera.h"

Player::Player() {
	node = NULL;
	moveAnim = false;
	doRotate = false, doTurn = false;
	doMove = false;
	fxAngle = 0.0, fyAngle = 0.0;
	exAngle = 0.0;
	position = vec3(0.0, 0.0, 0.0);
	camera = NULL;
	zoom = 10.0;
	speed = 0.0;
	atkPres = false, defPres = false;
}

void Player::setNode(AnimationNode* n, Camera* cam) { 
	if (node != n) {
		node = n;
		if (node) {
			fxAngle = node->getObject()->angley;
			fyAngle = 0.0;
			position = node->position;
			camera = cam;
			zoom = 10.0;
			speed = 0.0;
			atkPres = false, defPres = false;
			cameraAct();
		}
		exAngle = 0.0;
		doRotate = false, doTurn = false;
		doMove = false;
	}
}

void Player::run(int dir) {
	if (node) {
		if ((node->getObject()->isEnd() && !node->getObject()->isPlayOnce()) || node->getObject()->isDefaultAnim()) {
			node->getObject()->resetTime();
			moveAnim = true;
		}
		if (moveAnim && node->getObject()->getCurAnim() != 19) 
			node->getObject()->setCurAnim(19, false);
		if (node->getObject()->getCurAnim() == 19)
			node->getObject()->setMoving(true);
		float dr = 0.0;
		switch (dir) {
			case MNEAR:
				dr = 180.0;
				break;
			case MFAR:
				dr = 0.0;
				exAngle = 180.0;
				doRotate = true;
				break;
			case LEFT:
				dr = 270.0;
				exAngle = 90.0;
				doRotate = true;
				break;
			case RIGHT:
				dr = 90.0;
				exAngle = 270.0;
				doRotate = true;
				break;
		}
		float dir = angleToRadian(fxAngle + dr);
		position += vec3(sinf(dir), 0.0, cosf(dir)) * speed;
		doMove = true;
	}
}

void Player::idel() {
	if (node) {
		node->getObject()->setMoving(false);
		if (moveAnim) {
			node->getObject()->setCurAnim(node->getObject()->defaultAid, false);
			node->getObject()->resetTime();
			moveAnim = false;
		}
	}
}

void Player::switchAct(int target, bool once) {
	if (node) {
		int before = node->getObject()->aid;
		node->getObject()->setMoving(false);
		node->getObject()->setCurAnim(target, once);
		if (before != target || (node->getObject()->isEnd() && !node->getObject()->isPlayOnce()))
			node->getObject()->resetTime();
	}
	moveAnim = false;
}

void Player::resetPlayOnce() {
	if (node) node->getObject()->setPlayOnce(false);
}

void Player::attack() {
	if (node) switchAct(2, false);
}

void Player::defend() {
	if (node) switchAct(4, true);
}

void Player::crit() {
	if (node) switchAct(17, false);
}

void Player::kick() {
	if (node) switchAct(16, false);
}

void Player::jump() {
	if (node) switchAct(3, false);
}

void Player::turn(bool lr, float angle) {
	if (node) {
		if (lr) {
			float dAngle = fxAngle + angle;
			RestrictAngle(dAngle);
			fxAngle = dAngle;
		} else {
			float dAngle = fyAngle + angle;
			RestrictYAngle(dAngle);
			fyAngle = dAngle;
			if (fyAngle > 30.0) fyAngle = 30.0;
			else if (fyAngle < -30.0) fyAngle = -30.0;
		}
		doRotate = true;
		doTurn = true;
	}
}

void Player::resetExAngle() {
	if (exAngle != 0.0) {
		exAngle = 0.0;
		doRotate = true;
	}
}

bool Player::rotateAct() {
	if (doRotate) {
		if (node)
			node->rotateNodeObject(node->getObject()->anglex, fxAngle + exAngle, node->getObject()->anglez);
		doRotate = false;
		if(!doTurn) return false;
		else {
			doTurn = false;
			return true;
		}
	}
	return false;
}

bool Player::moveAct(const Scene* scene) {
	if (doMove) {
		if (node) {
			node->translateNode(position.x, position.y, position.z);
			scene->terrainNode->standObjectsOnGround(node);
			position = node->position;
		}
		doMove = false;
		return true;
	}
	return false;
}

void Player::cameraAct() {
	if (!camera || !node) return;
	vec4 pDir = rotateY(fxAngle) * rotateX(fyAngle) * UNIT_NEG_Z;
	vec3 dir = vec3(pDir.x, pDir.y, pDir.z).GetNormalized() * zoom;
	float gx = node->getObject()->transformsFull[0];
	float gy = node->getObject()->transformsFull[1] + ((AABB*)node->boundingBox)->sizey;
	float gz = node->getObject()->transformsFull[2];
	vec3 pos = vec3(gx, gy, gz) - dir;
	camera->setView(pos, dir);
}

void Player::keyDown(Input* input, const Scene* scene) { 
	int cid = -2;
	if (input->getBoards()[KEY_1]) cid = 1;
	if (input->getBoards()[KEY_2]) cid = 2;
	if (input->getBoards()[KEY_3]) cid = 3;
	if (input->getBoards()[KEY_V]) cid = -1;
	if (cid > -2) {
		input->setControl(cid);
		if(cid < 0) setNode(NULL, NULL);
		else setNode(scene->animPlayers[cid], scene->mainCamera);
	}
}

void Player::keyUp(Input* input) {
	if (!node) return;
	if (!input->getBoards()[KEY_W] && !input->getBoards()[KEY_S] && !input->getBoards()[KEY_A] && !input->getBoards()[KEY_D] &&
		!input->getBoards()[KEY_R] && !input->getBoards()[KEY_F] && !input->getBoards()[KEY_SPACE] && !atkPres && !defPres)
			idel();
}

void Player::controlAct(Input* input, const Scene* scene, const float velocity) {
	if (!node) return;

	speed = velocity;
	if (input->getBoards()[KEY_W])
		run(MNEAR);
	if (input->getBoards()[KEY_S])
		run(MFAR);
	if (input->getBoards()[KEY_A])
		run(LEFT);
	if (input->getBoards()[KEY_D])
		run(RIGHT);

	if (input->getBoards()[KEY_LEFT])
		turn(true, D_ROTATION);
	if (input->getBoards()[KEY_RIGHT])
		turn(true, -D_ROTATION);
	if (input->getBoards()[KEY_UP])
		turn(false, D_ROTATION);
	if (input->getBoards()[KEY_DOWN])
		turn(false, -D_ROTATION);

	if (input->getBoards()[KEY_W] && input->getBoards()[KEY_A])
		exAngle = 45.0;
	if (input->getBoards()[KEY_W] && input->getBoards()[KEY_D])
		exAngle = 315.0;
	if (input->getBoards()[KEY_S] && input->getBoards()[KEY_A])
		exAngle = 135.0;
	if (input->getBoards()[KEY_S] && input->getBoards()[KEY_D])
		exAngle = 225.0;
	if (!input->getBoards()[KEY_S] && !input->getBoards()[KEY_A] && !input->getBoards()[KEY_D])
		resetExAngle();

	bool isMove = moveAct(scene);
	bool isRotate = rotateAct();
	if(isMove || isRotate)
		cameraAct();

	if (atkPres) attack();
	if (defPres) defend();
	if (input->getBoards()[KEY_R])
		kick();
	if (input->getBoards()[KEY_F])
		crit();
	if (input->getBoards()[KEY_SPACE])
		jump();
}

void Player::mousePress(bool press, bool isMain) {
	if (!node) return;
	if (press) {
		if (isMain) atkPres = true;
		else defPres = true;
	} else {
		if (isMain) atkPres = false;
		else defPres = false;
	}
	if (!defPres) resetPlayOnce();
}

void Player::mouseAct(const float mouseX, const float mouseY, const float centerX, const float centerY) {
	if (!node) return;
	if (mouseX == centerX && mouseY == centerY) return;

	const static float cosdr = cos(A2R);
	const static float sindr = sin(A2R);
	const static float mag = 0.5 * 0.001f * R2A;
	float dxr = (centerX - mouseX) * mag;
	float dyr = (centerY - mouseY) * mag;
	turn(true, dxr * cosdr);
	turn(true, -dyr * sindr);
	turn(false, dxr * sindr);
	turn(false, dyr * cosdr);
	if(rotateAct())
		cameraAct();
}

void Player::wheelAct(float dz) {
	if (!node) return;
	zoom += dz;
	if (zoom < 5.0) zoom = 5.0;
	else if (zoom > 20.0) zoom = 20.0;
	cameraAct();
}