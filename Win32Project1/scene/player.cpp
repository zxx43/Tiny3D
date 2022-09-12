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
	isMove = false, isRotate = false;
	fyAngle = 0.0, exAngle = 0.0;
	basicQuat = vec4(0, 0, 0, 1);
	yQuat = vec4(0, 0, 0, 1);
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
			basicQuat = node->getObject()->rotateQuat;
			yQuat = vec4(0, 0, 0, 1);
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
		if (moveAnim && node->getObject()->getCurAnim() != "walk")
			node->getObject()->setCurAnim("walk", false);
		if (node->getObject()->getCurAnim() == "walk")
			node->getObject()->setMoving(true);
		switch (dir) {
			case MNEAR:
				break;
			case MFAR:
				exAngle = 180.0;
				doRotate = true;
				break;
			case LEFT:
				exAngle = 90.0;
				doRotate = true;
				break;
			case RIGHT:
				exAngle = 270.0;
				doRotate = true;
				break;
		}
		vec3 playerDir(-node->getObject()->rotateMat[8], -node->getObject()->rotateMat[9], -node->getObject()->rotateMat[10]);
		playerDir.Normalize();
		position += playerDir * speed;
		doMove = true;
	}
}

void Player::idel() {
	if (node) {
		node->getObject()->setMoving(false);
		if (moveAnim) {
			node->getObject()->setCurAnim(node->getObject()->defaultAname.data(), false);
			node->getObject()->resetTime();
			moveAnim = false;
		}
	}
}

void Player::switchAct(std::string target, bool once) {
	if (node) {
		std::string before = node->getObject()->getCurAnim();
		node->getObject()->setMoving(false);
		node->getObject()->setCurAnim(target.data(), once);
		if (before != target || (node->getObject()->isEnd() && !node->getObject()->isPlayOnce()))
			node->getObject()->resetTime();
	}
	moveAnim = false;
}

void Player::resetPlayOnce() {
	if (node) node->getObject()->setPlayOnce(false);
}

void Player::attack() {
	if (node) switchAct("attack3", false);
}

void Player::defend() {
	if (node) switchAct("block", true);
}

void Player::crit() {
	if (node) switchAct("attack1", false);
}

void Player::kick() {
	if (node) switchAct("kick", false);
}

void Player::jump() {
	if (node) switchAct("jump", false);
}

void Player::turn(bool lr, float angle) {
	if (node) {
		if (lr) {
			vec4 quat = Euler2Quat(vec3(0, angle, 0));
			basicQuat = MulQuat(quat, basicQuat);
		} else {
			float dAngle = fyAngle + angle;
			RestrictYAngle(dAngle);
			fyAngle = dAngle;
			if (fyAngle > 30.0) fyAngle = 30.0;
			else if (fyAngle < -30.0) fyAngle = -30.0;
			else {
				vec4 quat = Euler2Quat(vec3(angle, 0, 0));
				yQuat = MulQuat(quat, yQuat);
			}
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

bool Player::rotateAct(Scene* scene) {
	if (doRotate) {
		if (node) {
			vec4 rotQuat = basicQuat;
			if (exAngle != 0.0) {
				vec4 exQuat = Euler2Quat(vec3(0, exAngle, 0));
				rotQuat = MulQuat(exQuat, basicQuat);
			}
			node->rotateNodeObject(scene, rotQuat);
		}
		doRotate = false;
		if(!doTurn) return false;
		else {
			doTurn = false;
			return true;
		}
	}
	return false;
}

bool Player::moveAct(Scene* scene) {
	if (doMove) {
		if (node) {
			node->translateNode(scene, position.x, position.y, position.z);
		}
		doMove = false;
		return true;
	}
	return false;
}

void Player::cameraAct() {
	if (!camera || !node) return;
	vec4 camQuat = MulQuat(basicQuat, yQuat);
	mat4 rotMat = Quat2Mat(camQuat);
	vec3 pDir(rotMat[8], rotMat[9], rotMat[10]);
	vec3 dir = (-pDir).GetNormalized() * zoom;
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
		if (cid < 0 || cid >= scene->animPlayers.size() || scene->animPlayers[cid]->getObject()->animation->name != "ninja") {
			input->setControl(-1);
			setNode(NULL, NULL);
		} else {
			input->setControl(cid);
			setNode(scene->animPlayers[cid], scene->actCamera);
		}
	}
}

void Player::keyUp(Input* input) {
	if (!node) return;
	if (!input->getBoards()[KEY_W] && !input->getBoards()[KEY_S] && !input->getBoards()[KEY_A] && !input->getBoards()[KEY_D] &&
		!input->getBoards()[KEY_R] && !input->getBoards()[KEY_F] && !input->getBoards()[KEY_SPACE] && !atkPres && !defPres)
			idel();
}

void Player::controlAct(Input* input, Scene* scene, const float velocity) {
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

	isRotate = rotateAct(scene);
	isMove = moveAct(scene);

	if (atkPres) attack();
	if (defPres) defend();
	if (input->getBoards()[KEY_R])
		kick();
	if (input->getBoards()[KEY_F])
		crit();
	if (input->getBoards()[KEY_SPACE])
		jump();
}

void Player::updateCamera() {
	if (!node) return;
	if (isMove || isRotate)
		cameraAct();
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

void Player::mouseAct(Scene* scene, const float mouseX, const float mouseY, const float centerX, const float centerY) {
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
	if (rotateAct(scene))
		cameraAct();
}

void Player::wheelAct(float dz) {
	if (!node) return;
	zoom += dz;
	if (zoom < 5.0) zoom = 5.0;
	else if (zoom > 20.0) zoom = 20.0;
	cameraAct();
}