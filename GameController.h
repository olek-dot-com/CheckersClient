#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "NetworkClient.cpp"
class GameController
{
	sf::RenderWindow window;
	Board board;
	bool whiteMove = true;
	bool areYouWhite = true; //true - white, false - black
	bool wasPieceDestroyedByMe = false;
	bool wasPieceDestroyedByEnemy = false;
	bool wasMessageReceived = false;
	int whitePices = 12;
	int blackPices = 12;
	int whiteQueens = 0;
	int blackQueens = 0;
	CustomClient c;

public:
	GameController();
	void Start();
	void displayBoard();
	bool isMoveValid(int fromRow, int fromCol, int toRow, int toCol);
	void makeEnemysMove(int fromRow, int fromCol, int toRow, int toCol);
	void makeMove(int row, int col);
	bool checkEndResult();
	bool canIBeatNext(int row, int col);
	bool canEnemyBeatNext(int row, int col);
	bool queenChecker(int row, int col);
	void sendMessage(int isWhiteMove, int fromRow, int fromCol, int toRow, int toCol);
	void receiveMessage();
	//ruch bia³ego do kitu, damy sprawdziæ.

	// ruch dam trzeba dodaæ PieceType::BlackQueen i PieceType::WhiteQueen i zmieniæ odpowiednio funkcje isMoveValid i makeMove
};

