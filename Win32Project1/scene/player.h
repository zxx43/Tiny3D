#ifndef PLAYER_H_
#define PLAYER_H_

#include "../node/animationNode.h"

class Input;

class Player {
private:
	AnimationNode* node;
	bool moveAnim, doRotate, doMove;
	float fAngle, exAngle;
	float px, py, pz;
private:
	void run(int dir, Scene* scene);
	void switchAct(int target);
	void idel();
	void jump();
	void attack();
	void crit();
	void kick();
	void turn(float angle);
	void resetExAngle();
	void rotateAct();
	void moveAct(Scene* scene);
public:
	Player();
	~Player();
	void setNode(AnimationNode* n);
	AnimationNode* getNode() { return node; }
	void keyDown(Input* input);
	void keyUp(Input* input);
	void keyAct(Input* input, Scene* scene);
};

#endif