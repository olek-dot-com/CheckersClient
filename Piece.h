#pragma once
#include "PieceTypes.h"
class Piece
{
public:
	PieceType pieceType;
	int row;
	int col;
	void makeQueen();
};

