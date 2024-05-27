#include "Piece.h"

void Piece::makeQueen()
{
	if (this->pieceType == PieceType::WHITE) this->pieceType = PieceType::WHITEQUEEN;
	if (this->pieceType == PieceType::BLACK) this->pieceType = PieceType::BLACKQUEEN;
}
