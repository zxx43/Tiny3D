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
	virtual void initFaces();
public:
	Board();
	Board(const Board& rhs);
	virtual ~Board();
};

#endif /* BOARD_H_ */
