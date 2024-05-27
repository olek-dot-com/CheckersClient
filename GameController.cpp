#include "GameController.h"


GameController::GameController(): window(sf::VideoMode(SQUARE_SIZE* BOARD_SIZE, SQUARE_SIZE* BOARD_SIZE), "Checkers"){}
void GameController::Start()
{

    c.connect("127.0.0.1", 60000);
    bool bQuit = false;
    if (c.isConnected()) {
        window.clear();
        displayBoard();
        while (window.isOpen() and !bQuit) {
            sf::Event event;
            while (window.pollEvent(event)) {
                receiveMessage();
                if (event.type == sf::Event::Closed) {
                    window.close();
                    bQuit = true;
                }
                else if (event.type == sf::Event::MouseButtonPressed and (whiteMove == areYouWhite)) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        int col = mousePos.y / SQUARE_SIZE;
                        int row = mousePos.x / SQUARE_SIZE;
                        //std::cout << "Clicked on square (" << row << ", " << col << ")" << std::endl;
                        makeMove(row, col);
                        window.clear();
                        displayBoard();
                        checkEndResult();
                        if (checkEndResult()) window.close();
                    }
                }
               
            }
        }
	}else {
        std::cout << "Server down\n";
        bQuit = true;
    }
}

void GameController::displayBoard()
{
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board.square.setPosition(i * SQUARE_SIZE, j * SQUARE_SIZE);
            if ((i + j) % 2 == 0)
                board.square.setFillColor(sf::Color(255, 206, 158)); // Light square color
            else
                board.square.setFillColor(sf::Color(209, 139, 71)); // Dark square color
            window.draw(board.square);

            // Draw pieces
            if (board.getPiece(i, j) == PieceType::WHITE) {
                board.whitePiece.setPosition(i * SQUARE_SIZE + 10, j * SQUARE_SIZE + 10); // Ustawienie pozycji okrêgu
                window.draw(board.whitePiece);
            }
            else if (board.getPiece(i, j) == PieceType::BLACK) {
                board.blackPiece.setPosition(i * SQUARE_SIZE + 10, j * SQUARE_SIZE + 10); // Ustawienie pozycji okrêgu
                window.draw(board.blackPiece);
            }
            else if (board.getPiece(i, j) == PieceType::WHITEQUEEN) {
				board.whiteQueen.setPosition(i * SQUARE_SIZE + 10, j * SQUARE_SIZE + 10); // Ustawienie pozycji okrêgu
				window.draw(board.whiteQueen);
			}
            else if (board.getPiece(i, j) == PieceType::BLACKQUEEN) {
				board.blackQueen.setPosition(i * SQUARE_SIZE + 10, j * SQUARE_SIZE + 10); // Ustawienie pozycji okrêgu
				window.draw(board.blackQueen);
			}
        }
    }

    window.display();
}

// Funkcja sprawdzaj¹ca, czy ruch na pole (row, col) jest dozwolony z pozycji (fromRow, fromCol)
bool GameController::isMoveValid(int fromRow, int fromCol, int toRow, int toCol) {
    wasPieceDestroyedByMe = false;

    //Check whether the field is empty. If not, that's an invalid move for sure
    if (board.checkPieceFieldType(toRow,toCol) != PieceType::EMPTY) return false;

    //White Queen move
    if (board.checkPieceFieldType(fromRow, fromCol) == PieceType::WHITEQUEEN) {
        if (std::abs(toRow - fromRow) == 1 and std::abs(toCol - fromCol) == 1) {
            return true;
        }
        else if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
            if (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACK
                or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACKQUEEN) {
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                wasPieceDestroyedByMe = true;
                return true;
            }
        }
    }
    //Black Queen move
    if(board.checkPieceFieldType(fromRow, fromCol) == PieceType::BLACKQUEEN) {
        if (std::abs(toRow - fromRow) == 1 and std::abs(toCol - fromCol) == 1) {
            return true;
        }
        else if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
            if (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITE
                or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITEQUEEN) {
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                wasPieceDestroyedByMe = true;
                return true;
            }
            return true;
        }
	}

    //White move
    if (this->whiteMove) {

        //Can move only one field forward
        if (std::abs(toRow - fromRow) == 1 and toCol - fromCol == -1) {
            return true;
        }
        else if (std::abs(toRow - fromRow) == 1 and toCol - fromCol == 1) {
            //Mistake - moving backwards
            std::cout << "You can't move backwards!" << std::endl;
            return false;
        }
        else if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
            //There is a chance to beat a black piece
            if (/*board.checkPieceFieldType(toRow, toCol) == PieceType::EMPTY
                and */(board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACK
                or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACKQUEEN)) {
                   
                //Black piece beat
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                //Information about destroyed piece required in order to check if the next beat is possible
                wasPieceDestroyedByMe = true;
                
                //Information about the number of pieces
                if(board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACK) blackQueens--;
                else blackPices--;
                std::cout << "Black Piece destroyed." << std::endl << "Black pieces: " << blackPices << std::endl << "Black Queen/s: " << blackQueens << std::endl;
                
                return true;
            }
        }
        else {
            std::cout << "Other mistake." << std::endl;
            return false;
        }
    }
    //Black move
    else {
        if (std::abs(toRow - fromRow) == 1 and toCol - fromCol == 1) {
            return true;
        }
        else if (std::abs(toRow - fromRow) == 1 and toCol - fromCol == -1) {
            //Mistake - moving backwards
            std::cout << "You cannot move backwards!" << std::endl;
            return false;
        }   
        else if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
            //There is a chance to beat a white piece
            if (/*board.checkPieceFieldType(toRow, toCol) == PieceType::EMPTY
                and */(board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITE
                or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITEQUEEN)) {

                //White piece beat
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                wasPieceDestroyedByMe = true;

                //Information about the number of pieces
                if (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITE) whiteQueens--;
				else whitePices--;
                std::cout << "White piece destroyed." << std::endl << "White pieces: " << whitePices << std::endl << "White Queen/s: " << whiteQueens << std::endl;
                
                return true;
            }
        }
        else {
            std::cout << "Other mistake." << std::endl;
            return false;
        }
    }
}

void GameController::makeEnemysMove(int fromRow, int fromCol, int toRow, int toCol)
{
    //It gets the color of the player who made the move, whiteMove is not changed since the message was received
    wasPieceDestroyedByEnemy = false;
    board.setSelectedPiecePosition(fromRow, fromCol);
    sf::Vector2i selectedPos = board.getSelectedPiecePosition();

    //std::cout << "white move: " << whiteMove << std::endl;
    //std::cout << "areYouWhite: " << areYouWhite << std::endl;
 
    board.setSelectedPiecePosition(-1, -1); // Reset selected piece position
    if (this->whiteMove) {
        std::cout << "White move to beat" << std::endl;
        if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
            std::cout << "White inside if statement" << std::endl;
            //There is a chance to beat a black piece
            if (board.checkPieceFieldType(toRow, toCol) == PieceType::EMPTY
                and (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACK
                    or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACKQUEEN)) {
                std::cout << "white inside indide if statement" << std::endl;
                //Black piece beat
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                //Information about destroyed piece required in order to check if the next beat is possible
                wasPieceDestroyedByEnemy = true;

                //Information about the number of pieces
                if (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::BLACK) blackQueens--;
                else blackPices--;
                std::cout << "Black Piece destroyed." << std::endl << "Black pieces: " << blackPices << std::endl << "Black Queen/s: " << blackQueens << std::endl;
            }
        }
    }
    else if (std::abs(toRow - fromRow) == 2 and std::abs(toCol - fromCol) == 2) {
        std::cout << "Black move to beat" << std::endl;
            //There is a chance to beat a white piece
            if (board.checkPieceFieldType(toRow, toCol) == PieceType::EMPTY
                and (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITE
                    or board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITEQUEEN)) {
                std::cout << "Black inside indide if statement" << std::endl;
                //White piece beat
                board.destroyPiece((fromRow + toRow) / 2, (fromCol + toCol) / 2);
                wasPieceDestroyedByEnemy = true;

                //Information about the number of pieces
                if (board.checkPieceFieldType((fromRow + toRow) / 2, (fromCol + toCol) / 2) == PieceType::WHITE) whiteQueens--;
                else whitePices--;
                std::cout << "White piece destroyed." << std::endl << "White pieces: " << whitePices << std::endl << "White Queen/s: " << whiteQueens << std::endl;
            }
    }
    std::cout << "Moved piece from (" << selectedPos.x << ", " << selectedPos.y << ") to (" << toRow << ", " << toCol << ")" << std::endl;
    board.movePiece(selectedPos.x, selectedPos.y, toRow, toCol);
    if (queenChecker(toRow, toCol) and whiteMove) {
        board.setPiece(toRow, toCol, PieceType::WHITEQUEEN);
    }
    else if (queenChecker(toRow, toCol) and !whiteMove) {
        board.setPiece(toRow, toCol, PieceType::BLACKQUEEN);
    }
   // std::cout << "toRow: " << toRow << " toCol: " << toCol << " fromRow: " << fromRow << " fromCol: " << fromCol << std::endl;
    std::cout << "Can beat next: " << canEnemyBeatNext(toRow, toCol) << std::endl;
    std::cout << "Was piece destroyed: " << wasPieceDestroyedByEnemy << std::endl;
    if (canEnemyBeatNext(toRow, toCol) and wasPieceDestroyedByEnemy) {
        window.clear();
        displayBoard();
        checkEndResult();
        receiveMessage();

    }
    else {
        //If the opponent can't beat next, the player changes
        whiteMove = whiteMove ? 0 : 1;
        checkEndResult();
        window.clear();
        displayBoard();
    }

}

void GameController::makeMove(int row, int col)
{
    if (board.getSelectedPiecePosition() == sf::Vector2i(-1, -1)) { // Jeœli nie wybrano jeszcze pionka
        if (board.getPiece(row, col) != PieceType::EMPTY) { // Jeœli pole zawiera pionka

            if (whiteMove and (board.getPiece(row, col) == PieceType::WHITE or board.getPiece(row,col) == PieceType::WHITEQUEEN)) {
                board.setSelectedPiecePosition(row, col); // Ustaw pozycjê wybranego pionka
                std::cout << "Selected piece at (" << row << ", " << col << ")" << std::endl;

            }
            else if (!whiteMove and (board.getPiece(row, col) == PieceType::BLACK or board.getPiece(row,col) == PieceType::BLACKQUEEN))
            {
                board.setSelectedPiecePosition(row, col); // Ustaw pozycjê wybranego pionka
                std::cout << "Selected piece at (" << row << ", " << col << ")" << std::endl;

            }
            else {
                std::cout << "Invalid piece" << std::endl;
                board.setSelectedPiecePosition(-1, -1);
            }

        }
    }
    else { // Jeœli wybrano ju¿ pionka
        sf::Vector2i selectedPos = board.getSelectedPiecePosition();
        if (this->isMoveValid(selectedPos.x, selectedPos.y, row, col)) {
            board.movePiece(selectedPos.x, selectedPos.y, row, col);
            
            board.setSelectedPiecePosition(-1, -1); // Zresetuj pozycjê wybranego pionka
            std::cout << "Moved piece from (" << selectedPos.x << ", " << selectedPos.y << ") to (" << row << ", " << col << ")" << std::endl;
            
            if (queenChecker(row, col) and whiteMove) {
                board.setPiece(row, col, PieceType::WHITEQUEEN);
            }
            else if(queenChecker(row, col) and !whiteMove){
                board.setPiece(row, col, PieceType::BLACKQUEEN);
            }

            if (canIBeatNext(row, col) and wasPieceDestroyedByMe) {
                makeMove(row, col);
                //std::cout << "You have to beat next piece in funct finished" << std::endl;
                //White move sent to the server from the state it was during the move it was made
                sendMessage(whiteMove, selectedPos.x, selectedPos.y, row, col);
                //The playre stays the same
                whiteMove = whiteMove ? 1 : 0;
            }
            else {
				//White move sent to the server from the state it was during the move it was made
                sendMessage(whiteMove, selectedPos.x, selectedPos.y, row, col);
                //The player changes
                whiteMove = whiteMove ? 0 : 1;
            }

        }
        else
        {
                std::cout << "Invalid move" << std::endl;
                board.setSelectedPiecePosition(-1, -1);
            
        }
    }
}

bool GameController::canIBeatNext(int row, int col)
{
    // sprawdzenie nie "wyjedzie" poza plansze, bicie mo¿liwe w ka¿d¹ stronê
    if (row > 1 and row < 6 and col > 1 and col < 6) { 
        
        if (whiteMove){
            if((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
               (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else
        {
            //complete code as above but for black pieces
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }

    if((col == 6 or col == 7) and row > 1 and row < 6){
        if (whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }
    if((col == 1 or col == 0) and row > 1 and row < 6){
        if (whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
                if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                    (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITEQUEEN))) {
                    return true;
                }
                return false;
        }
    }
    if((row == 1 or row == 0) and col > 1 and col < 6){
        if (whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }
    if((row == 6 or row == 7) and col > 1 and col < 6){
        if (whiteMove) {
            if ((board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK)) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE)) {
                return true;
            }
            return false;
        }
    }

    return false;
}
bool GameController::canEnemyBeatNext(int row, int col)
{
    // sprawdzenie nie "wyjedzie" poza plansze, bicie mo¿liwe w ka¿d¹ stronê
    if (row > 1 and row < 6 and col > 1 and col < 6) {

        if (!whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else
        {
            //complete code as above but for black pieces
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }

    if ((col == 6 or col == 7) and row > 1 and row < 6) {
        if (!whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }
    if ((col == 1 or col == 0) and row > 1 and row < 6) {
        if (!whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }
    if ((row == 1 or row == 0) and col > 1 and col < 6) {
        if (!whiteMove) {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col + 1) == PieceType::BLACKQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACK or board.checkPieceFieldType(row + 1, col - 1) == PieceType::BLACKQUEEN))) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row + 2, col + 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col + 1) == PieceType::WHITEQUEEN)) or
                (board.checkPieceFieldType(row + 2, col - 2) == PieceType::EMPTY and (board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITE or board.checkPieceFieldType(row + 1, col - 1) == PieceType::WHITEQUEEN))) {
                return true;
            }
            return false;
        }
    }
    if ((row == 6 or row == 7) and col > 1 and col < 6) {
        if (!whiteMove) {
            if ((board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col + 1) == PieceType::BLACK) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col - 1) == PieceType::BLACK)) {
                return true;
            }
            return false;
        }
        else {
            if ((board.checkPieceFieldType(row - 2, col + 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col + 1) == PieceType::WHITE) or
                (board.checkPieceFieldType(row - 2, col - 2) == PieceType::EMPTY and board.checkPieceFieldType(row - 1, col - 1) == PieceType::WHITE)) {
                return true;
            }
            return false;
        }
    }

    return false;
}

bool GameController::queenChecker(int col, int row)
{
if (whiteMove and row == 0) {
		board.setPiece(col, row, PieceType::WHITEQUEEN);
		whitePices--;
		whiteQueens++;
        std::cout << "White Queen" << std::endl;
        return true;
	}
	else if (!whiteMove and row == 7) {
		board.setPiece(col, row, PieceType::BLACKQUEEN);
		blackPices--;
		blackQueens++;
        std::cout << "Black Queen" << std::endl;
        return true;
    }
    return false;
}

void GameController::sendMessage(int whiteMove, int fromRow, int fromCol, int toRow, int toCol)
{
    c.messageAll(whiteMove, fromCol, fromRow, toRow, toCol);
}

void GameController::receiveMessage()
{

    if (!c.Incoming().empty()) {
        auto msg = c.Incoming().pop_front().msg;

        switch (msg.header.id) {
            case CustomMsgTypes::ServerAccept:
            {
                std::cout << "Server Accepted Connection\n";
                uint8_t areYouWhiteUint;
                msg >> areYouWhiteUint;
                areYouWhite = static_cast<int>(areYouWhiteUint);
                std::cout << "areYouWhite: " << areYouWhite << std::endl;
                break;
            }

            case CustomMsgTypes::ServerMessage: {

                uint8_t areYouWhiteUint, whiteMoveUint, formRowUint, formColUint, toRowUint, toColUint;
                msg >> toRowUint >> toColUint >> formRowUint >> formColUint >> whiteMoveUint;

                whiteMove = static_cast<int>(whiteMoveUint);
                int formRow = static_cast<int>(formRowUint);
                int formCol = static_cast<int>(formColUint);
                int toRow = static_cast<int>(toRowUint);
                int toCol = static_cast<int>(toColUint);
               // std::cout << "received server message in if statement" << std::endl << std::endl;
                //std::cout << " whiteMove: " << whiteMove << " fromRow: " << formRow << " fromCol: " << formCol << " toRow: " << toRow << " toCol: " << toCol << std::endl;
                makeEnemysMove(formCol,formRow, toCol, toRow);
                wasMessageReceived = true;
                break;
            }
             default:
				std::cout << "Unrecognized message: " << std::endl;
				break;
        }
    }
}

bool GameController::checkEndResult()
{
    if (whitePices == 0 and whiteQueens == 0) { 
        std::cout << "Black Wins, window closed" << std::endl;
        Sleep(5);
        return true;
    }
    if (blackPices == 0 and blackQueens == 0) {
        std::cout << "White Wins, window closed" << std::endl;
        Sleep(5);
        return true;
    }
    if (blackPices == 0 and whitePices == 0 and whiteQueens == 1 and blackQueens == 1) {
        std::cout << "DRAW, window closed" << std::endl;
        Sleep(5);
        return true;
    }
    //sprawdziæ zablokowanie pionków 
    return false;

}
