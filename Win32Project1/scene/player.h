#ifndef PLAYER_H_
#define PLAYER_H_

#include "../node/animationNode.h"

class Input;
class Camera;

class Player {
private:
	AnimationNode* node;
	bool moveAnim, doRotate, doTurn, doMove;
	bool isMove, isRotate;
	float fxAngle, fyAngle, exAngle;
	vec4 basicQuat;
	vec3 position;
	Camera* camera;
	float zoom, speed;
	bool atkPres, defPres;
private:
	void run(int dir);
	void switchAct(std::string target, bool once);
	void resetPlayOnce();
	void idel();
	void jump();
	void attack();
	void crit();
	void kick();
	void defend();
	void turn(bool lr, float angle);
	void resetExAngle();
	bool rotateAct(Scene* scene);
	bool moveAct(Scene* scene);
	void cameraAct();
public:
	Player();
	~Player() {}
	void setNode(AnimationNode* n, Camera* cam);
	AnimationNode* getNode() { return node; }
	void keyDown(Input* input, const Scene* scene);
	void keyUp(Input* input);
	void controlAct(Input* input, Scene* scene, const float velocity);
	void updateCamera();
	void mouseAct(Scene* scene, const float mouseX, const float mouseY, const float centerX, const float centerY);
	void mousePress(bool press, bool isMain);
	void wheelAct(float dz);
	void setPosition(const vec3& newPos) { if (node) position = newPos; }
};

#endif