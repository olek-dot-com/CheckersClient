#pragma once
#define BOARD_SIZE 8
#define SQUARE_SIZE 80
#include <iostream>
#include <SFML/Graphics.hpp>
#include "PieceTypes.h"

class GameController;
class Board {
private:
    PieceType board[BOARD_SIZE][BOARD_SIZE];
    sf::Vector2i selectedPiecePos; // Pozycja wybranego pionka


public:
    sf::RectangleShape square;
    sf::CircleShape whitePiece;
    sf::CircleShape blackPiece;
    sf::CircleShape whiteQueen;
    sf::CircleShape blackQueen;
    Board();
    PieceType getPiece(int row, int col);

    void setPiece(int row, int col, PieceType type);
    
    PieceType checkPieceFieldType(int row, int col);
    
    // Funkcja do przesuwania pionka z pozycji (fromRow, fromCol) na pole (toRow, toCol)
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);

    // Funkcja do ustawienia pozycji wybranego pionka
    void setSelectedPiecePosition(int row, int col);

    // Funkcja do pobrania pozycji wybranego pionka
    sf::Vector2i getSelectedPiecePosition();
    void destroyPiece(int row, int col);
};

