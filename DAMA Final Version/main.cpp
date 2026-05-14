#include "raylib.h"
#include <string>
#include <fstream>
#include <iostream>

const int BOARD_SIZE = 800; // Size of the game board in pixels
const int CELL_SIZE = 90;   // Size of each individual cell in pixels
const int offSet = 80;      // Offset to position the board correctly on the screen

// Enumeration for cell types, representing different states of the board's cells
enum cellType {
    emptyCell,          // Represents an empty cell with no pieces
    player1Qorki,       // Represents a normal piece for Player 1
    player2Qorki,       // Represents a normal piece for Player 2
    player1KingQorki,   // Represents a "King" piece for Player 1
    player2KingQorki    // Represents a "King" piece for Player 2
};

// Declaring textures for player pieces
Texture2D player1Texture;    // Texture for Player 1's regular piece
Texture2D player2Texture;    // Texture for Player 2's regular piece
Texture2D player1KingTexture;// Texture for Player 1's King piece
Texture2D player2KingTexture;// Texture for Player 2's King piece

// Game sounds
Sound move;    // Moving Piece sound
Sound capture; // capturing piece sound
Sound win;     // Winning Game Sound

// Struct representing a single cell on the board, including its row, column, and type
struct Cell {
    int row;                    // Row position of the cell on the board
    int col;                    // Column position of the cell on the board
    int cellType;               // Type of the cell, represented by the cellType enum
    int cellSize = CELL_SIZE;   // Size of the cell, defaulting to CELL_SIZE
};

// Struct representing the properties of the game board
struct Board {
    int boardWidth;          // Width of the board in pixels
    int boardHeight;         // Height of the board in pixels
    Color boardColor;        // Color of the board's background
};

// Struct to encapsulate the game state, including the board, player info, and cells
struct Game {
    Board board;             // The board itself
    Cell cellInfo[8][8];     // 8x8 grid representing all the cells on the board
    std::string playerOneName; // Name of Player 1
    std::string playerTwoName; // Name of Player 2
    bool isPlayerOneTurn;    // Boolean indicating if it's Player 1's turn
    int playerOneScore;      // Score of Player 1
    int playerTwoScore;      // Score of Player 2
    Cell* selectedCell;      // Pointer to the currently selected cell, if any
};

// Function Prototypes with Detailed Comments

/**
 * Initializes the game state by setting initial properties like player names, 
 * loading textures, and setting up the board and pieces.
 * @param game A reference to the Game object that holds the entire game state.
 */
void initGame(Game& game);

/**
 * Initializes the game board properties like size and color.
 * @param board A reference to the Board object representing the game board.
 */
void initBoard(Board& board);

/**
 * Draws the entire game board as a rectangle on the screen using Raylib's DrawRectangle function.
 * @param board A reference to the Board object representing the game board.
 */
void drawBoard(Board board);

/**
 * Initializes the type of a cell based on its position on the board. Depending on the 
 * row and column, it assigns a player piece or an empty cell.
 * @param row The row number of the cell on the board.
 * @param col The column number of the cell on the board.
 * @return Returns an int value representing the type of the cell (empty, player1Qorki, etc.).
 */
int initCellType(int row, int col);

/**
 * Draws all the cells on the game board grid. Each cell is drawn with a color (black/white)
 * depending on its position, simulating a checkered pattern.
 * @param cell A 2D array of Cell objects that represent the game grid.
 */
void drawCellsOnBoard(Cell cell[8][8]);

/**
 * Determines and returns the color of a cell based on its position on the board. 
 * Cells alternate between black and white.
 * @param cell The Cell object for which the color is being determined.
 * @return Returns a Color (BLACK or WHITE) based on the cell's position.
 */
Color getCellColor(Cell cell);

/**
 * Draws all the player pieces (Qorkis) on the board. It checks each cell, 
 * and if it contains a player piece, it draws the corresponding texture.
 * @param cell A 2D array of Cell objects representing the entire game board.
 */
void drawQorki(Cell cell[8][8]);

/**
 * Updates the game state by handling user input (mouse clicks), managing 
 * piece selection, and moving pieces.
 * @param game A reference to the Game object representing the current game state.
 */
void updateGame(Game& game);

/**
 * Finds and returns the cell on the board that corresponds to the given mouse click coordinates.
 * @param x The x-coordinate of the mouse click.
 * @param y The y-coordinate of the mouse click.
 * @param cells A 2D array of Cell objects representing the entire game board.
 * @return Returns a pointer to the Cell object that was clicked, or nullptr if the click is outside the board.
 */
Cell* getCell(int x, int y, Cell cells[8][8]);

/**
 * Moves a selected piece (Qorki) from one cell to another, updates the game board,
 * and switches the turn to the other player.
 * @param selectedCell A pointer to the currently selected cell containing the piece to move.
 * @param targetCell A pointer to the cell where the piece is being moved.
 * @param game A reference to the Game object representing the current game state.
 */
void moveQorki(Cell* selectedCell, Cell* targetCell, Game& game);

/**
 * Checks whether a move from a selected cell to a target cell is valid based on 
 * the game rules, including piece type and movement rules.
 * @param selectedCell A pointer to the currently selected cell.
 * @param targetCell A pointer to the target cell.
 * @param game A reference to the Game object representing the current game state.
 * @return Returns true if the move is valid, false otherwise.
 */
bool isMoveValid(Cell* selectedCell, Cell* targetCell, Game& game);

/**
 * Highlights the valid moves for the currently selected piece (Qorki) by visually 
 * marking the cells where the piece can legally move.
 * @param selectedCell A pointer to the currently selected cell.
 * @param game A reference to the Game object representing the current game state.
 */
void highlightValidMoves(Cell* selectedCell, Game& game);

/**
 * Promotes a regular piece (Qorki) to a King piece if it reaches the opponent's back row.
 * @param cell A pointer to the cell containing the piece to be promoted.
 */
void promoteToKing(Cell* cell);

/**
 * Determines if the currently selected piece (Qorki) can capture an opponent's piece 
 * based on its current position on the board and the game rules.  
 * @param selectedCell A pointer to the currently selected cell that contains the piece to check.
 * @param game A reference to the Game object representing the current game state, including the board and player turns.
 * @return Returns true if the selected piece can capture an opponent's piece, false otherwise.
 */
bool canCapture(Cell* selectedCell, Game& game);

/**
 * Draws the current score of both players on the screen.
 * @param game A reference to the Game object representing the current game state.
 */
void drawScore(Game& game);

/**
 * Saves the current game state to a file so that the game can be resumed later.
 * The game is saved in text format.
 * @param game A reference to the Game object representing the current game state.
 */
void saveGame(const Game& game);

/**
 * Loads a saved game state from a file. The game state is read from a text file.
 * @param game A reference to the Game object where the loaded state will be stored.
 */
void loadGame(Game& game);

/**
 * Checks if the game is over by verifying whether either player has any pieces left on the board.
 * @param game A reference to the Game object representing the current game state.
 * @return Returns true if the game is over (one player has no pieces left), false otherwise.
 */
bool isGameOver(Game& game);

/**
 * Displays the "Game Over" screen with a message indicating the winner. This is shown when 
 * the game has ended and one player has no pieces left.
 * @param game A reference to the Game object representing the current game state.
 */
void displayGameOver(Game& game);

/**
 * Displays the main menu of the game, where the player can choose to start a new game, 
 * load a saved game, or exit.
 * @return Returns an integer representing the player's menu choice (1 = new game, 2 = load game, 0 = exit).
 */
int showMainMenu();

/*
 *Loads textures and sounds
*/
void loadTextures();

/**
 *Removes Textures and sounds from memory
 */
void unloadTextures();


bool newGame = true;   // flag for new game
bool firstTurn = true; // flag for checking turn


int main() {
    // Initialize window and set properties
    std::cout << "Initializing window..." << std::endl;
    InitWindow(BOARD_SIZE - offSet, BOARD_SIZE, "DAMA");  // Set up the game window with a specific size and title
    SetTargetFPS(60);  // Set the frame rate to 60 FPS for smoother gameplay
    SetExitKey(KEY_NULL);  // Disable the default exit key (ESC) to manage it manually
    std::cout << "Window initialized. Target FPS set to 60." << std::endl;

    // Initialize game state
    std::cout << "Initializing game state..." << std::endl;
    Game game;  // Create an instance of the Game struct to manage game elements
    bool gameOver = false;  // Initialize the game over state

    // Show the main menu at the start
    std::cout << "Displaying main menu..." << std::endl;
    int menuOption = showMainMenu();  // Show main menu and get player's choice
    if (menuOption == 0) {
        // Player chose to exit the game
        std::cout << "Player chose to exit from the main menu." << std::endl;
        unloadTextures();  // Unload textures to free memory
        CloseWindow();  // Close the game window
        return 0;  // Exit the program
    } else if (menuOption == 1) {
        // Player chose to start a new game
        std::cout << "Player chose to start a new game." << std::endl;
      
    } else if (menuOption == 2) {
        // Player chose to load a saved game
        std::cout << "Player chose to load a saved game." << std::endl;
        loadGame(game);  // Load the saved game state
        newGame = false;  // Indicate that this is not a new game
    }

    initGame(game);  // Initialize the game

    // Main game loop, runs until the player decides to close the game window
    while (!WindowShouldClose()) {
        BeginDrawing();  // Begin rendering the frame
        ClearBackground(RAYWHITE);  // Clear the background to white

        if (gameOver) {
            // If the game is over, display the Game Over screen
            std::cout << "Game is over. Displaying Game Over screen." << std::endl;
            displayGameOver(game);  // Display the game over message
            PlaySound(win);  // Play the victory sound

            // Restart the game if the player presses Enter
            if (IsKeyPressed(KEY_ENTER)) {
                std::cout << "Enter key pressed. Restarting game." << std::endl;
                newGame = true;
                initGame(game);  // Reinitialize the game for a new session
                gameOver = false;  // Reset game over status
            }
        } else {
            // Game is not over, proceed with normal rendering and logic
            drawBoard(game.board);  // Draw the game board
            drawCellsOnBoard(game.cellInfo);  // Draw all the cells on the board
            drawQorki(game.cellInfo);  // Draw the player pieces on the board
            drawScore(game);  // Display the players' scores
            updateGame(game);  // Update game logic based on player interactions


            // If a cell is selected, highlight valid moves for the selected piece
            if (game.selectedCell != nullptr) {
                //std::cout << "A piece is selected. Highlighting valid moves." << std::endl;
                highlightValidMoves(game.selectedCell, game);
            }

            // Check if ESC key is pressed to open the menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                std::cout << "ESC key pressed. Displaying menu." << std::endl;
                saveGame(game);  // Save the current game state
                menuOption = showMainMenu();  // Show the main menu

                if (menuOption == 0) {
                    // Player chose to exit from the menu
                    std::cout << "Player chose to exit from the menu." << std::endl;
                    saveGame(game);  // Save game before exiting
                    unloadTextures();  // Unload textures to free memory
                    CloseWindow();  // Close the game window
                    return 0;  // Exit the program
                } else if (menuOption == 1) {
                    // Player chose to start a new game
                    std::cout << "Player chose to start a new game from menu." << std::endl;
                    newGame = true;
                    initGame(game);  // Initialize a new game
                    gameOver = false;  // Reset game over status
                } else if (menuOption == 2) {
                    // Player chose to load a saved game
                    std::cout << "Player chose to load a saved game from menu." << std::endl;
                    loadGame(game);  // Load the saved game state
                }
            }

            // Check if 'L' key is pressed to load the game
            if (IsKeyPressed(KEY_L)) {
                std::cout << "'L' key pressed. Loading game." << std::endl;
                loadGame(game);  // Load the saved game state
            }

            // Check if the game is over
            if (isGameOver(game)) {
                std::cout << "Game over condition met." << std::endl;
                gameOver = true;  // Set the game over flag
            }
        }

        EndDrawing();  // End rendering the frame
    }

    // Save the game before exiting
    std::cout << "Saving game before exit..." << std::endl;
    saveGame(game);
    
    // Unload textures and close the window before exiting
    std::cout << "Unloading textures and closing window." << std::endl;
    unloadTextures();
    CloseWindow();

    std::cout << "Exiting program." << std::endl;
    return 0;
}



// Initialization functions

// Initializes the game state
void initGame(Game& game) {
     std::cout << "Initializing game state..." << std::endl;

    // Unload previous textures to avoid conflicts
    std::cout << "Unloading existing textures before reinitializing..." << std::endl;
    unloadTextures();

    // Initialize the game board
    initBoard(game.board);
    std::cout << "Game board initialized." << std::endl;

    if (newGame){
    // Initialize player name variables to hold the user input
    char playerOneNameInput[32] = "\0";  // Buffer for Player 1's name
    char playerTwoNameInput[32] = "\0";  // Buffer for Player 2's name
    int letterCountP1 = 0;  // Counter for Player 1's name length
    int letterCountP2 = 0;  // Counter for Player 2's name length

    bool nameInputDone = false;
    bool enteringPlayerOne = true; // True if we're entering Player 1's name, false for Player 2

    // Input loop for player names
    while (!nameInputDone) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (enteringPlayerOne) {
            // Input for Player 1's name
            DrawText("Enter Player 1 Name: ", 100, 100, 25, BLACK);
            DrawText(playerOneNameInput, 400, 100, 25, DARKGRAY);
            
            // Capture player name input
            if (letterCountP1 < 31) {  // Max length for name is 31 chars
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125)) { // Acceptable key range
                        playerOneNameInput[letterCountP1] = (char)key;
                        letterCountP1++;
                        playerOneNameInput[letterCountP1] = '\0';  // Null terminate the string
                    }
                    key = GetCharPressed();
                }
            }

            // When Enter is pressed for Player 1, move to Player 2 input
            if (IsKeyPressed(KEY_ENTER) && letterCountP1 > 0) {
                enteringPlayerOne = false;  // Move to Player 2 input phase
            }

        } else {
            // Input for Player 2's name
            DrawText("Enter Player 2 Name: ", 100, 200, 25, BLACK);
            DrawText(playerTwoNameInput, 400, 200, 25, DARKGRAY);
            
            // Capture player name input
            if (letterCountP2 < 31) {  // Max length for name is 31 chars
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125)) { // Acceptable key range
                        playerTwoNameInput[letterCountP2] = (char)key;
                        letterCountP2++;
                        playerTwoNameInput[letterCountP2] = '\0';  // Null terminate the string
                    }
                    key = GetCharPressed();
                }
            }

            // Proceed when Enter is pressed for Player 2's name
            if (IsKeyPressed(KEY_ENTER) && letterCountP2 > 0) {
                nameInputDone = true;  // End input loop
            }
        }

        EndDrawing();
    }

    // Set player names in the game
    game.playerOneName = playerOneNameInput; 
    game.playerTwoName = playerTwoNameInput;

    } else {
         // Set Player 1's and Player 2's as place holders before loading
         game.playerOneName = "Player 1";
         game.playerTwoName = "Player 2";
    }

    // Initialize player scores
    game.playerOneScore = 0;
    game.playerTwoScore = 0;
    std::cout << "Player scores initialized to 0." << std::endl;

    // Set the turn to Player 1
    game.isPlayerOneTurn = true;
    std::cout << "Player 1's turn starts." << std::endl;

    // No cell is selected initially
    game.selectedCell = nullptr;

    loadTextures();

    // Initialize each cell on the board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            game.cellInfo[i][j].row = i; // Set the row of the cell
            game.cellInfo[i][j].col = j; // Set the column of the cell
            game.cellInfo[i][j].cellType = initCellType(i, j); // Initialize cell type
        }
    }
    

    

}


// Initializes the board properties
void initBoard(Board& board) {
    board.boardWidth = BOARD_SIZE; // Set board width
    board.boardHeight = BOARD_SIZE; // Set board height
    board.boardColor = DARKGRAY; // Set board color
    std::cout << "Board initialized with width and height of " << BOARD_SIZE << "." << std::endl;
}

// Initializes the cell type based on row and column
int initCellType(int row, int col) {
    if ((row + col) % 2 == 1) {  // Pieces should be on dark squares only
        if (row < 3) return player2Qorki; // Player 2's pieces
        if (row > 4) return player1Qorki; // Player 1's pieces
    }
    return emptyCell; // Empty cell
}

// Drawing functions

// Draws the board on the screen
void drawBoard(Board board) {
    DrawRectangle(0, offSet, board.boardWidth, board.boardHeight, board.boardColor); // Draw the board rectangle
}

// Draws all cells on the board
void drawCellsOnBoard(Cell cell[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Color cellColor = getCellColor(cell[i][j]); // Get the cell color
            DrawRectangle(cell[i][j].col * cell[i][j].cellSize, 
                          cell[i][j].row * cell[i][j].cellSize + offSet, 
                          cell[i][j].cellSize, 
                          cell[i][j].cellSize, 
                          cellColor); // Draw the cell
        }
    }
}

// Returns the color of the cell based on its position
Color getCellColor(Cell cell) {
    if ((cell.row + cell.col) % 2 == 1) return WHITE; // White squares
    return BLACK; // Black squares
}

// Draws the Qorkis (pieces) on the board
void drawQorki(Cell cell[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (cell[i][j].cellType != emptyCell) { // Only draw non-empty cells
                float x = cell[i][j].col * cell[i][j].cellSize; // X position
                float y = cell[i][j].row * cell[i][j].cellSize + offSet; // Y position
                int size = cell[i][j].cellSize; // Cell size
                // Source rectangle for texture (full texture)
                Rectangle sourceRect = { 0, 0, (float)player1Texture.width, (float)player1Texture.height }; // Full texture
                // Destination rectangle for drawing
                Rectangle destRect = { x, y, (float)size, (float)size }; // Destination size, scaled to fit the cell
                Vector2 origin = { 0, 0 }; // Origin point for texture (top-left)

                // Draw the correct texture based on the cell type
                if (cell[i][j].cellType == player1Qorki) {
                    DrawTexturePro(player1Texture, sourceRect, destRect, origin, 0.0f, WHITE);
                } else if (cell[i][j].cellType == player2Qorki) {
                    DrawTexturePro(player2Texture, sourceRect, destRect, origin, 0.0f, WHITE);
                } else if (cell[i][j].cellType == player1KingQorki) {
                    DrawTexturePro(player1KingTexture, sourceRect, destRect, origin, 0.0f, WHITE);
                } else if (cell[i][j].cellType == player2KingQorki) {
                    DrawTexturePro(player2KingTexture, sourceRect, destRect, origin, 0.0f, WHITE);
                }
            }
        }
    }
}

// Gets the cell based on mouse click coordinates
Cell* getCell(int x, int y, Cell cells[8][8]) {
    //std::cout << "Getting cell for coordinates (" << x << ", " << y << ")..." << std::endl;
    int col = x / cells[0][0].cellSize; // Calculate column based on X coordinate
    int row = (y - offSet) / cells[0][0].cellSize; // Calculate row based on Y coordinate
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        std::cout << "Cell found at [" << row << "][" << col << "]." << std::endl;
        return &cells[row][col]; // Return the cell if within bounds
    }
    std::cout << "Coordinates out of bounds. Returning nullptr." << std::endl;
    return nullptr; // Return nullptr if out of bounds
}

// Moves the Qorki (piece) from selected to target cell
void moveQorki(Cell* selectedCell, Cell* targetCell, Game& game) {
    // Check if selectedCell or targetCell is null, exit function if true
    if (selectedCell == nullptr || targetCell == nullptr) {
        std::cout << "Selected or target cell is null. Move aborted." << std::endl;
        return;
    }

    // Move the piece from selectedCell to targetCell
    targetCell->cellType = selectedCell->cellType;  // Move the piece type to the target cell
    selectedCell->cellType = emptyCell;  // Empty the selected cell
    PlaySound(move);  // Play move sound for a regular move
    std::cout << "Moved piece from (" << selectedCell->row << ", " << selectedCell->col 
              << ") to (" << targetCell->row << ", " << targetCell->col << ")." << std::endl;

    // Calculate the difference in rows to check for a capture (2-row jump)
    int rowDiff = targetCell->row - selectedCell->row;
    if (abs(rowDiff) == 2) {  // A capture occurs when the row difference is 2
        int jumpedRow = (selectedCell->row + targetCell->row) / 2;  // Row of the jumped piece
        int jumpedCol = (selectedCell->col + targetCell->col) / 2;  // Column of the jumped piece

        // Remove the jumped piece
        game.cellInfo[jumpedRow][jumpedCol].cellType = emptyCell;
        PlaySound(capture);  // Play capture sound when a piece is captured
        std::cout << "Captured piece at (" << jumpedRow << ", " << jumpedCol << ")." << std::endl;

        // Update the score based on the current player
        if (game.isPlayerOneTurn) {
            game.playerOneScore++;
            std::cout << "Player 1 score: " << game.playerOneScore << std::endl;
        } else {
            game.playerTwoScore++;
            std::cout << "Player 2 score: " << game.playerTwoScore << std::endl;
        }

        // After a capture, check if another capture is possible
        if (canCapture(targetCell, game)) {
            game.selectedCell = targetCell;  // Allow the player to make another capture
            //firstTurn = false;
            std::cout << "Another capture possible, staying on the same player." << std::endl;
            return;  // Do not switch turns, as another capture is possible
        }
    }

    // Check if the piece should be promoted to a King
    if ((game.isPlayerOneTurn && targetCell->row == 0) || 
        (!game.isPlayerOneTurn && targetCell->row == 7)) {
        promoteToKing(targetCell);  // Promote to King if conditions are met
        std::cout << "Promoted piece at (" << targetCell->row << ", " << targetCell->col << ") to King." << std::endl;
    }

    // Switch the turn to the other player
    game.isPlayerOneTurn = !game.isPlayerOneTurn;
    //firstTurn = true;
    std::cout << "Turn switched. It is now Player " << (game.isPlayerOneTurn ? "1" : "2") << "'s turn." << std::endl;
    game.selectedCell = nullptr;  // Deselect the piece after the move
}


// Check if a piece can make a valid move or capture
bool isMoveValid(Cell* selectedCell, Cell* targetCell, Game& game) {
    // Check if selectedCell or targetCell is null, return false if true
    if (selectedCell == nullptr || targetCell == nullptr) {
        std::cout << "Selected or target cell is null. Move invalid." << std::endl;
        return false;
    }

    int rowDiff = targetCell->row - selectedCell->row;  // Row difference
    int colDiff = targetCell->col - selectedCell->col;  // Column difference

    // Ensure the target cell is empty
    if (targetCell->cellType != emptyCell) {
       // std::cout << "Target cell at (" << targetCell->row << ", " << targetCell->col << ") is not empty. Move invalid." << std::endl;
        return false;
    }

    // Check for a capturing move (2-row jump)
    if (abs(rowDiff) == 2 && abs(colDiff) == 2) {
        int jumpedRow = (selectedCell->row + targetCell->row) / 2;  // Row of the jumped piece
        int jumpedCol = (selectedCell->col + targetCell->col) / 2;  // Column of the jumped piece
        Cell* jumpedCell = &game.cellInfo[jumpedRow][jumpedCol];  // The cell that is jumped over

        // Check if the player is capturing an opponent's piece
        if (game.isPlayerOneTurn && (jumpedCell->cellType == player2Qorki || jumpedCell->cellType == player2KingQorki)) {
            // Normal pieces can only move forward
            if (selectedCell->cellType == player1Qorki && rowDiff == -2) {
               // std::cout << "Valid capture for Player 1." << std::endl;
                return true;
            }
            // King pieces can move in any direction
            if (selectedCell->cellType == player1KingQorki) {
               // std::cout << "Valid capture for Player 1's King." << std::endl;
                return true;
            }
        }
        if (!game.isPlayerOneTurn && (jumpedCell->cellType == player1Qorki || jumpedCell->cellType == player1KingQorki)) {
            // Normal pieces can only move forward
            if (selectedCell->cellType == player2Qorki && rowDiff == 2) {
               // std::cout << "Valid capture for Player 2." << std::endl;
                return true;
            }
            // King pieces can move in any direction
            if (selectedCell->cellType == player2KingQorki) {
               // std::cout << "Valid capture for Player 2's King." << std::endl;
                return true;
            }
        }
    }

    // Regular moves are only allowed if no capture is possible
    if (abs(rowDiff) == 1 && abs(colDiff) == 1) {
        // Normal move logic for Qorkis and Kings
        if (game.isPlayerOneTurn && selectedCell->cellType == player1Qorki && rowDiff == -1) {
           // std::cout << "Valid move for Player 1." << std::endl;
            return true;
        }
        if (!game.isPlayerOneTurn && selectedCell->cellType == player2Qorki && rowDiff == 1) {
           // std::cout << "Valid move for Player 2." << std::endl;
            return true;
        }
        if (selectedCell->cellType == player1KingQorki || selectedCell->cellType == player2KingQorki) {
          //  std::cout << "Valid move for King." << std::endl;
            return true;
        }
    }

    //std::cout << "Move invalid for selected piece." << std::endl;   for invalid moves
    return false;
}
// Updates the game state, handling selection and movement of pieces
void updateGame(Game& game) {
    // Check if the left mouse button is pressed
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int mouseX = GetMouseX();  // Get the X coordinate of the mouse
        int mouseY = GetMouseY();  // Get the Y coordinate of the mouse
        Cell* clickedCell = getCell(mouseX, mouseY, game.cellInfo);  // Get the clicked cell based on mouse coordinates

        // Check if a cell was clicked
        if (clickedCell != nullptr) {
            // If no piece is selected, select the clicked piece
            if (game.selectedCell == nullptr && clickedCell->cellType != emptyCell) {
                // Validate if the clicked piece belongs to the current player
                if ((game.isPlayerOneTurn && (clickedCell->cellType == player1Qorki || clickedCell->cellType == player1KingQorki)) ||
                    (!game.isPlayerOneTurn && (clickedCell->cellType == player2Qorki || clickedCell->cellType == player2KingQorki))) {
                    game.selectedCell = clickedCell;  // Select the clicked cell
                    std::cout << "Selected piece at (" << clickedCell->row << ", " << clickedCell->col << ")." << std::endl;
                } else {
                    std::cout << "Clicked piece does not belong to the current player." << std::endl;
                }
            }
            // If a piece is selected, try to move it
            else if (game.selectedCell != nullptr) {
                // Check if the move is valid before moving
                if (isMoveValid(game.selectedCell, clickedCell, game)) {
                    moveQorki(game.selectedCell, clickedCell, game);  // Move the piece if valid
                } else {
                   // std::cout << "Move invalid. Deselecting piece." << std::endl;
                    game.selectedCell = nullptr;  // Deselect if the move is invalid
                }
            }
        }
    }
}

/// Highlights valid moves for the selected piece
void highlightValidMoves(Cell* selectedCell, Game& game) {
    // Return if no piece is selected
    if (selectedCell == nullptr) {
       // std::cout << "No piece selected to highlight valid moves." << std::endl;
        return;
    }

    // Loop through potential moves (2 rows/columns in each direction)
    for (int rowDiff = -2; rowDiff <= 2; rowDiff++) {
        for (int colDiff = -2; colDiff <= 2; colDiff++) {
            int targetRow = selectedCell->row + rowDiff;  // Calculate target row
            int targetCol = selectedCell->col + colDiff;  // Calculate target column
            
            // Ensure the target cell is within board bounds
            if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {
                Cell* targetCell = &game.cellInfo[targetRow][targetCol];  // Get the target cell
                
                // If the move is valid, highlight the entire square
                if (isMoveValid(selectedCell, targetCell, game)) {
                    DrawRectangle(targetCell->col * targetCell->cellSize, 
                                  (targetCell->row * targetCell->cellSize) + offSet, 
                                  targetCell->cellSize, 
                                  targetCell->cellSize, 
                                  Fade(GREEN, 0.5f));  // Use Fade to give it transparency if needed
                    //std::cout << "Highlighting valid move to (" << targetRow << ", " << targetCol << ")." << std::endl;
                }
            }
        }
    }
}

// Promotes a Qorki (piece) to a King
void promoteToKing(Cell* cell) {
    // Check the type of the cell and promote to King accordingly
    if (cell->cellType == player1Qorki) {
        cell->cellType = player1KingQorki;  // Promote to Player 1's King
        std::cout << "Promoted Player 1's Qorki to King." << std::endl;
    } else if (cell->cellType == player2Qorki) {
        cell->cellType = player2KingQorki;  // Promote to Player 2's King
        std::cout << "Promoted Player 2's Qorki to King." << std::endl;
    }
}

// Checks if a piece can capture an opponent's piece
bool canCapture(Cell* selectedCell, Game& game) {
    // Ensure that a piece is selected and it's not an empty cell
    if (selectedCell->cellType == emptyCell) {
        std::cout << "No piece selected, cell is empty." << std::endl;  
        return false;
    }

    // Check if the selected piece belongs to the current player
    bool isPlayer1Turn = game.isPlayerOneTurn;
    if ((isPlayer1Turn && (selectedCell->cellType == player2Qorki || selectedCell->cellType == player2KingQorki)) ||
        (!isPlayer1Turn && (selectedCell->cellType == player1Qorki || selectedCell->cellType == player1KingQorki))) {
        std::cout << "Cannot capture, wrong player's piece selected." << std::endl;  
        return false;
    }

    // Define the directions in which a piece can move (diagonal)
    int directions[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };  // Diagonal movement directions
    int row = selectedCell->row;  // Current row of selected piece
    int col = selectedCell->col;  // Current column of selected piece

    // Loop through all four possible diagonal directions
    for (int i = 0; i < 4; i++) {
        int dr = directions[i][0];  // Row direction (either +1 or -1)
        int dc = directions[i][1];  // Column direction (either +1 or -1)

        // Check if the piece can move two cells in the current direction (must be within board boundaries)
        int targetRow = row + 2 * dr;  // Target row after jump
        int targetCol = col + 2 * dc;  // Target column after jump
        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {
            Cell* adjacentCell = &game.cellInfo[row + dr][col + dc];  // Cell that is diagonally adjacent to the selected piece
            Cell* targetCell = &game.cellInfo[targetRow][targetCol];  // Cell that the piece would land on after the jump

            // Check if the adjacent cell contains an opponent's piece
            bool isOpponentPiece = (isPlayer1Turn && (adjacentCell->cellType == player2Qorki || adjacentCell->cellType == player2KingQorki)) ||
                                   (!isPlayer1Turn && (adjacentCell->cellType == player1Qorki || adjacentCell->cellType == player1KingQorki));

            // Check if the target cell (two steps away) is empty
            bool isTargetCellEmpty = (targetCell->cellType == emptyCell);

            // If an opponent's piece is adjacent and the target cell is empty, a capture is possible
            if (isOpponentPiece && isTargetCellEmpty) {
                std::cout << "Capture possible at (" << targetRow << ", " << targetCol 
                          << ") from (" << row << ", " << col << ")" << std::endl;  
                return true;  // Capture is valid
            }
        }
    }

    // No valid captures found
    std::cout << "No capture possible for the selected piece at (" << row << ", " << col << ")." << std::endl; 
    return false;  // Capture not possible
}


// Draws the current score on the screen with player names
void drawScore(Game& game) {
    // Display Player 1's score with their name
    DrawText(TextFormat("%s : %d", game.playerOneName.c_str(), game.playerOneScore), 20, 20, 30, RED);
    
    // Display Player 2's score with their name
    DrawText(TextFormat("%s : %d", game.playerTwoName.c_str(), game.playerTwoScore), 500, 20, 30, BLACK);
    
    // Display the message for accessing the menu
    DrawText("Press 'ESC' for Menu", 220, 20, 20, GRAY);
}

// Save the game state to a file
void saveGame(const Game& game) {
    std::ofstream saveFile("game_save.txt"); // Open a file to save game state

    if (!saveFile.is_open()) {
        std::cerr << "Error saving the game!" << std::endl;
        return;
    }

    // Save player names and scores
    saveFile << game.playerOneName << std::endl;
    saveFile << game.playerTwoName << std::endl;
    saveFile << game.playerOneScore << std::endl;
    saveFile << game.playerTwoScore << std::endl;
    saveFile << game.isPlayerOneTurn << std::endl;

    // Save the board state
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            saveFile << game.cellInfo[i][j].cellType << " ";
        }// Save each cell type
        saveFile << std::endl;// New line for next row
    }

    saveFile.close();// Close the file
    std::cout << "Game saved successfully!" << std::endl;
}

// Load the game state from a file
void loadGame(Game& game) {
    std::ifstream loadFile("game_save.txt");// Open the saved game file


    if (!loadFile.is_open()) {
        std::cerr << "Error loading the game!" << std::endl;
        return;
    }

    // Load player names and scores
    std::getline(loadFile, game.playerOneName);// Load Player 1 name
    std::getline(loadFile, game.playerTwoName);// Load Player 2 name
    loadFile >> game.playerOneScore;// Load Player 1 score
    loadFile >> game.playerTwoScore;// Load Player 2 score
    loadFile >> game.isPlayerOneTurn;// Load turn information

    // Load the board state
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            loadFile >> game.cellInfo[i][j].cellType;// Load each cell type
        }
    }

    loadFile.close();// Close the file
    std::cout << "Game loaded successfully!" << std::endl;
    
}
//checks if the game is over 
bool isGameOver(Game& game) {
    int playerOneQorkiCount = 0;
    int playerTwoQorkiCount = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (game.cellInfo[i][j].cellType == player1Qorki || game.cellInfo[i][j].cellType == player1KingQorki) {
                playerOneQorkiCount++;
            } else if (game.cellInfo[i][j].cellType == player2Qorki || game.cellInfo[i][j].cellType == player2KingQorki) {
                playerTwoQorkiCount++;
            }
        }
    }

    return (playerOneQorkiCount == 0 || playerTwoQorkiCount == 0);
}

// Displays the game over screen with the winning player's name
void displayGameOver(Game& game) {
    const char* winner;
    
    // Determine the winner based on scores
    if (game.playerOneScore > game.playerTwoScore) {
        winner = TextFormat("%s Wins!", game.playerOneName.c_str());  // Player 1 wins
    } else if (game.playerTwoScore > game.playerOneScore) {
        winner = TextFormat("%s Wins!", game.playerTwoName.c_str());  // Player 2 wins
    } else {
        winner = "It's a Draw!";  // In case of a tie
    }
    PlaySound(win);
    // Display the winner text in the middle of the screen
    DrawText(winner, 270, 300, 40, GREEN);
    
    
    // Display the prompt to start a new game
    DrawText("Press ENTER to start new Game", 200, 400, 20, DARKGRAY);
}

// Main menu function displays the necessary keys to play the game
int showMainMenu() {
    const int titleFontSize = 40;
    const int optionFontSize = 20;
    const int startYPosition = 200;
    const int optionSpacing = 50;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("DAMA GAME", 230, startYPosition, titleFontSize, BLACK);
        DrawText("Press ENTER to start a new game", 200, startYPosition + optionSpacing * 3, optionFontSize, DARKGRAY);
        DrawText("Press L to Resume game", 200, startYPosition + optionSpacing * 4, optionFontSize, DARKGRAY);
        DrawText("Press SPACE to exit", 200, startYPosition + optionSpacing * 5, optionFontSize, DARKGRAY);

        if (IsKeyPressed(KEY_ENTER)) {
            return 1;  // Start a new game
        }
        if (IsKeyPressed(KEY_L)) {
            return 2;  // Load a saved game
        }
        if (IsKeyPressed(KEY_SPACE)) {
            return 0;  // Exit the game
        }

        EndDrawing();
    }
    return 0;
}

   void loadTextures(){

         // Load textures for player pieces
    player1Texture = LoadTexture("Graphics\\redpiece.png");  // Texture for Player 1's piece
    player2Texture = LoadTexture("Graphics\\blackpiece.png"); // Texture for Player 2's piece
    player1KingTexture = LoadTexture("Graphics\\redking.png"); // Texture for Player 1's King
    player2KingTexture = LoadTexture("Graphics\\blackking.png"); // Texture for Player 2's King
    std::cout << "Player textures loaded." << std::endl;

    //Load audio files
    InitAudioDevice();

    move = LoadSound("Sounds\\movepiece.ogg");
    capture = LoadSound("Sounds\\capture.ogg");
    win = LoadSound("Sounds\\win.ogg");
} 
void unloadTextures() {
    
    UnloadTexture(player1Texture);
    UnloadTexture(player2Texture);
    UnloadTexture(player1KingTexture);
    UnloadTexture(player2KingTexture);

     UnloadSound(move);
    UnloadSound(capture);
    UnloadSound(win);

    CloseAudioDevice();

    }