#include "player.h"
#include "../constants/constants.h"
#include "../util/util.h"
#include "../input/input.h"

Player::Player() {
	node = NULL;
	moveAnim = false;
	doRotate = false;
	doMove = false;
	fAngle = 0.0;
	exAngle = 0.0;
	px = 0.0, py = 0.0, pz = 0.0;
}

Player::~Player() {

}

void Player::setNode(AnimationNode* n) { 
	if (node != n) {
		node = n;
		if (node) {
			fAngle = node->getObject()->angley;
			px = node->position.x;
			py = node->position.y;
			pz = node->position.z;
		}
		exAngle = 0.0;
		doRotate = false;
		doMove = false;
	}
}

void Player::run(int dir, Scene* scene) {
	if (node) {
		if (node->getObject()->isDefaultAnim()) {
			node->getObject()->resetTime();
			moveAnim = true;
		}
		if (moveAnim) node->getObject()->setCurAnim(19);
		float dr = 0.0, speed = 0.075;
		switch (dir) {
			case MNEAR:
				dr = 180.0;
				break;
			case MFAR:
				dr = 0.0;
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
		float dir = angleToRadian(fAngle + dr);
		px += sinf(dir) * speed;
		pz += cosf(dir) * speed;
		doMove = true;
	}
}

void Player::idel() {
	if (moveAnim) {
		if (node) {
			node->getObject()->setCurAnim(node->getObject()->defaultAid);
			node->getObject()->resetTime();
		}
		moveAnim = false;
	}
}

void Player::switchAct(int target) {
	int before = node->getObject()->aid;
	node->getObject()->setCurAnim(target);
	if (before != target)
		node->getObject()->resetTime();
	moveAnim = false;
}

void Player::jump() {
	if (node) switchAct(3);
}

void Player::attack() {
	if (node) switchAct(2);
}

void Player::crit() {
	if (node) switchAct(17);
}

void Player::kick() {
	if (node) switchAct(16);
}

void Player::turn(float angle) {
	if (node) {
		float dAngle = fAngle + angle;
		RestrictAngle(dAngle);
		fAngle = dAngle;
		doRotate = true;
	}
}

void Player::resetExAngle() {
	if (exAngle != 0.0) {
		exAngle = 0.0;
		doRotate = true;
	}
}

void Player::rotateAct() {
	if (doRotate) {
		if (node)
			node->rotateNodeObject(node->getObject()->anglex, fAngle + exAngle, node->getObject()->anglez);
		doRotate = false;
	}
}

void Player::moveAct(Scene* scene) {
	if (doMove) {
		if (node) {
			node->translateNode(px, py, pz);
			scene->terrainNode->standObjectsOnGround(node);
			px = node->position.x;
			py = node->position.y;
			pz = node->position.z;
		}
		doMove = false;
	}
}

void Player::keyDown(Input* input) {
	if (input->getBoards()[KEY_V]) {
		setNode(NULL);
		input->setControl(-1);
	}
	if (input->getBoards()[KEY_1])
		input->setControl(1);
	if (input->getBoards()[KEY_2])
		input->setControl(2);
	if (input->getBoards()[KEY_3])
		input->setControl(3);
}

void Player::keyUp(Input* input) {
	if (!node) return;
	if (!input->getBoards()[KEY_W] && !input->getBoards()[KEY_S] && !input->getBoards()[KEY_A] && !input->getBoards()[KEY_D] &&
		!input->getBoards()[KEY_R] && !input->getBoards()[KEY_F] && !input->getBoards()[KEY_G] && !input->getBoards()[KEY_SPACE])
			idel();
}

void Player::keyAct(Input* input, Scene* scene) {
	if (!node) return;

	if (input->getBoards()[KEY_W])
		run(MNEAR, scene);
	if (input->getBoards()[KEY_S])
		run(MFAR, scene);
	if (input->getBoards()[KEY_A])
		run(LEFT, scene);
	if (input->getBoards()[KEY_D])
		run(RIGHT, scene);
	moveAct(scene);

	if (input->getBoards()[KEY_Q])
		turn(D_ROTATION);
	if (input->getBoards()[KEY_E])
		turn(-D_ROTATION);

	if (input->getBoards()[KEY_W] && input->getBoards()[KEY_A])
		exAngle = 45.0;
	if (input->getBoards()[KEY_W] && input->getBoards()[KEY_D])
		exAngle = 315.0;
	if (input->getBoards()[KEY_S] && input->getBoards()[KEY_A])
		exAngle = 315.0;
	if (input->getBoards()[KEY_S] && input->getBoards()[KEY_D])
		exAngle = 45.0;
	if (!input->getBoards()[KEY_A] && !input->getBoards()[KEY_D])
		resetExAngle();
	rotateAct();

	if (input->getBoards()[KEY_R])
		kick();
	if (input->getBoards()[KEY_F])
		attack();
	if (input->getBoards()[KEY_G])
		crit();
	if (input->getBoards()[KEY_SPACE])
		jump();
}