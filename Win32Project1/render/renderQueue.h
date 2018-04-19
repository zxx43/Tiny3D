#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>
#include "../instance/instance.h"

class RenderQueue {
private:
	std::vector<Node*> queue;
	std::map<Mesh*, Instance*> instanceQueue;
private:
	void pushObjectToInstance(Object* object, Camera* camera, const VECTOR3D& eye, bool singleSide);
public:
	RenderQueue();
	~RenderQueue();
	void copyData(RenderQueue* src);
	void push(Node* node);
	void flush();
	void deleteInstance(Mesh* mesh);
	void draw(Camera* camera,const VECTOR3D& eye,Render* render, RenderState* state);
	void animate(long startTime, long currentTime);
};

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera);

#endif
