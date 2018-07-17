/*
 * board.h
 *
 *  Created on: 2017-4-9
 *      Author: a
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "mesh.h"

class Board: public Mesh {
private:
	float baseX, baseY, baseZ;
	float biasX, biasY;
private:
	virtual void initFaces();
public:
	Board();
	Board(float sizex, float sizey, float sizez);
	Board(float sizex, float sizey, float sizez, float offx, float offy);
	Board(const Board& rhs);
	virtual ~Board();
};

#endif /* BOARD_H_ */
