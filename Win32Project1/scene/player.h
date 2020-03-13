#ifndef PLAYER_H_
#define PLAYER_H_

#include "../node/animationNode.h"

class Input;
class Camera;

class Player {
private:
	AnimationNode* node;
	bool moveAnim, doRotate, doTurn, doMove;
	float fxAngle, fyAngle, exAngle;
	vec3 position;
	Camera* camera;
	float zoom, speed;
	bool atkPres, defPres;
private:
	void run(int dir);
	void switchAct(int target, bool once);
	void resetPlayOnce();
	void idel();
	void jump();
	void attack();
	void crit();
	void kick();
	void defend();
	void turn(bool lr, float angle);
	void resetExAngle();
	bool rotateAct();
	bool moveAct(const Scene* scene);
	void cameraAct();
public:
	Player();
	~Player() {}
	void setNode(AnimationNode* n, Camera* cam);
	AnimationNode* getNode() { return node; }
	void keyDown(Input* input, const Scene* scene);
	void keyUp(Input* input);
	void controlAct(Input* input, const Scene* scene, const float velocity);
	void mouseAct(const float mouseX, const float mouseY, const float centerX, const float centerY);
	void mousePress(bool press, bool isMain);
	void wheelAct(float dz);
};

#endif