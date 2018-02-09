#ifndef RENDER_QUEUE_H_
#define RENDER_QUEUE_H_

#include "render.h"
#include "../node/node.h"
#include <stdlib.h>

class RenderQueue {
private:
	std::vector<Node*> queue;
public:
	RenderQueue();
	~RenderQueue();
	void copyData(RenderQueue* src);
	void push(Node* node);
	void flush();
	void draw(Camera* camera,Render* render, RenderState* state);
	void animate(long startTime, long currentTime);
};

void pushNodeToQueue(RenderQueue* queue, Node* node, Camera* camera, bool simple);

#endif
