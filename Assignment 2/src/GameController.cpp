#include "GameController.h"
#include <fstream>
#include <iostream>
#include <chrono>

bool GameController::play(BlockFall &game, const string &commands_file) {

    ifstream commandsFile(commands_file);
    game.leaderboard.read_from_file(game.leaderboard_file_name);
    if (!commandsFile.is_open()) {
        std::cerr << "Error opening the commands file: " << commands_file << std::endl;
        return false; // Unable to open the commands file
    }
    string command;
    while (game.active_rotation != nullptr) {
        game.activeBlockCol = 0;
        game.activeBlockRow = 0;
        while (getline(commandsFile, command)) {
            if (game.isCollisionOnEntry(game.active_rotation, game.activeBlockRow, game.activeBlockCol)) {
                time_t currentTime = getCurrentTime();
                auto *currentEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name);
                game.leaderboard.insert_new_entry(currentEntry);
                game.leaderboard.write_to_file(game.leaderboard_file_name);
                std::cout << "GAME OVER!\n"
                             "Next block that couldn't fit:" << std::endl;
                for (int i = 0; i < game.active_rotation->shape.size(); ++i) {
                    for (int j = 0; j < game.active_rotation->shape[0].size(); ++j) {
                        if (game.active_rotation->shape[i][j] == 1) {
                            cout << occupiedCellChar;
                        } else {
                            cout << unoccupiedCellChar;
                        }
                    }
                    cout << endl;
                }

                std::cout << "\nFinal grid and score:\n" << std::endl;
                cout << "Score: " << game.current_score << endl;
                cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
                game.printGrid();
                game.leaderboard.print_leaderboard();
                return false; //GAME OVER COLLISION DETECTED
            }
            // Process each command dynamically
            if (command == "PRINT_GRID") {
                displayGridWithActiveBlock(game);
            } else if (command == "ROTATE_RIGHT") {
                game.rotateActiveBlockClockwise();
            } else if (command == "ROTATE_LEFT") {
                game.rotateActiveBlockCounterClockwise();
            } else if (command == "MOVE_RIGHT") {
                game.moveActiveBlockRight();
            } else if (command == "MOVE_LEFT") {
                game.moveActiveBlockLeft();
            } else if (command == "DROP") {
                game.dropActiveBlock();
                if (game.gravity_mode_on) {
                    game.gravity_mode_on = false;
                    game.toggleGravity();
                }
                game.handlePowerUp();
                game.handleFullRows();
                if (game.active_rotation == nullptr) {
                    std::cout << "YOU WIN!\n"
                                 "No more blocks.\n"
                                 "Final grid and score:\n" << std::endl;
                    cout << "Score: " << game.current_score << endl;
                    cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
                    game.printGrid();
                    time_t currentTime = getCurrentTime();
                    auto *currentEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name);
                    game.leaderboard.insert_new_entry(currentEntry);
                    game.leaderboard.write_to_file(game.leaderboard_file_name);
                    game.leaderboard.print_leaderboard();
                    return true; // Game win no more blocks.
                }
            } else if (command == "GRAVITY_SWITCH") {
                game.toggleGravity();
                game.specialHandleRowsForGravity();
            } else {
                std::cout << "Unknown command: " << command << std::endl;
                // Handle unknown commands gracefully (continue processing other commands)
            }
        }
        if (commandsFile.eof()) {
            std::cout << "GAME FINISHED!\n"
                         "No more commands.\n"
                         "Final grid and score:\n" << std::endl;
            cout << "Score: " << game.current_score << endl;
            cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
            game.printGrid();
            time_t currentTime = getCurrentTime();
            auto *currentEntry = new LeaderboardEntry(game.current_score, currentTime, game.player_name);
            game.leaderboard.insert_new_entry(currentEntry);
            game.leaderboard.write_to_file(game.leaderboard_file_name);
            game.leaderboard.print_leaderboard();
            return true;
        }
    }
    commandsFile.close();
    return true;
}

void GameController::displayGridWithActiveBlock(const BlockFall &game) {
    cout << "Score: " << game.current_score << endl;
    cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << endl;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            // Calculate the starting position of the active block within the grid
            int blockRow = i - game.activeBlockRow;
            int blockCol = j - game.activeBlockCol;

            // Check if the current position is within the bounds of the active block
            if (blockRow >= 0 && blockRow < game.active_rotation->shape.size() &&
                blockCol >= 0 && blockCol < game.active_rotation->shape[0].size() &&
                game.active_rotation->shape[blockRow][blockCol]) {
                // Print the active block cell without changing the grid values
                cout << occupiedCellChar;
            } else {
                // Print the grid cell
                if (game.grid[i][j] == 1) {
                    cout << occupiedCellChar;
                } else {
                    cout << unoccupiedCellChar;
                }
            }
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}

time_t GameController::getCurrentTime() {
    // Get the current system time point
    auto now = std::chrono::system_clock::now();

    // Convert the time point to a time_t value
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    return currentTime;
}





