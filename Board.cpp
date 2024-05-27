#include "Board.h"

Board::Board()
    : square(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE)), whitePiece(SQUARE_SIZE / 2 - 10),
    blackPiece(SQUARE_SIZE / 2 - 10), whiteQueen(SQUARE_SIZE / 2 - 10), blackQueen(SQUARE_SIZE / 2 - 10)
{
    whitePiece.setFillColor(sf::Color::White);
    blackPiece.setFillColor(sf::Color::Black);
    square.setOutlineThickness(1);
	square.setOutlineColor(sf::Color::Black);
    blackQueen.setFillColor(sf::Color(45, 45, 45)); // Set gray color
    whiteQueen.setFillColor(sf::Color(185, 185, 185)); // Set gray color

    // Initialize the board with pieces
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if ((i + j) % 2 == 0 && j < 3)
                board[i][j] = PieceType::BLACK;
            else if ((i + j) % 2 == 0 && j > 4)
                board[i][j] = PieceType::WHITE;
            else
                board[i][j] = PieceType::EMPTY;
        }
    }
    selectedPiecePos = sf::Vector2i(-1, -1); // Inicjalizacja pozycji wybranego pionka poza plansz¹. 
}

PieceType Board::getPiece(int row, int col)
{
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
        return board[row][col];
    else
        return PieceType::EMPTY;
}

void Board::setPiece(int row, int col, PieceType type)
{
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE)
        board[row][col] = type;
}

PieceType Board::checkPieceFieldType(int row, int col)
{
    return board[row][col];
}

void Board::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = PieceType::EMPTY;
}

void Board::setSelectedPiecePosition(int row, int col)
{
    selectedPiecePos.x = row;
    selectedPiecePos.y = col;
}

sf::Vector2i Board::getSelectedPiecePosition()
{
    return selectedPiecePos;
}

void Board::destroyPiece(int row, int col)
{
    setPiece(row, col, PieceType::EMPTY);
    std::cout << "Destroyed piece";
}

