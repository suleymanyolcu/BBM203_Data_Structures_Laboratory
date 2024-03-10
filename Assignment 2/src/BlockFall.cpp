#include <fstream>
#include <iostream>
#include <sstream>
#include "BlockFall.h"

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on,
                     const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
}

BlockFall::~BlockFall() {
    while (initial_block != nullptr) {
        Block *currentBlock = initial_block;
        initial_block = initial_block->next_block;

        // Delete rotations
        Block *rotation = currentBlock->right_rotation;
        while (rotation != currentBlock) {
            Block *nextRotation = rotation->right_rotation;
            rotation->left_rotation->right_rotation = rotation->right_rotation;
            rotation->right_rotation->left_rotation = rotation->left_rotation;
            delete rotation;
            rotation = nextRotation;
        }

        // Delete the current block
        currentBlock->right_rotation= nullptr;
        currentBlock->left_rotation= nullptr;
        delete currentBlock;
        active_rotation= nullptr;
    }
}

void BlockFall::initialize_grid(const string &input_file) {
    ifstream inputFile(input_file);
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            istringstream iss(line);
            vector<int> row;
            int value;
            while (iss >> value) {
                row.push_back(value);
            }
            grid.push_back(row);
        }
        rows = grid.size();
        cols = grid[0].size();
        inputFile.close();
    }
}

void BlockFall::read_blocks(const string &input_file) {
    ifstream inputFile(input_file);
    if (!inputFile.is_open()) {
        cerr << "Error opening the blocks file: " << input_file << endl;
        exit(EXIT_FAILURE);
    }
    Block *previousBlock = nullptr;
    string line;
    while (getline(inputFile, line)) {
        if (line.empty()) {
            // Empty line indicates the end of a block
            continue;
        }
        Block *currentBlock = new Block();
        stringstream ss(line);
        char c;
        vector<bool> row;
        while (ss >> c) {
            if (c == '1') {
                row.push_back(true);
            } else if (c == '0') {
                row.push_back(false);
            }
        }
        currentBlock->shape.push_back(row);
        while (getline(inputFile, line) && !line.empty()) {
            stringstream ss(line);
            row.clear();
            while (ss >> c) {
                if (c == '1') {
                    row.push_back(true);
                } else if (c == '0') {
                    row.push_back(false);
                }
            }
            currentBlock->shape.push_back(row);
        }
        if (previousBlock != nullptr) {
            previousBlock->next_block = currentBlock;
            previousBlock->right_rotation->next_block = currentBlock;
            previousBlock->right_rotation->right_rotation->next_block = currentBlock;
            previousBlock->right_rotation->right_rotation->right_rotation->next_block = currentBlock;

        } else {
            initial_block = currentBlock;
            active_rotation = initial_block;
        }

        previousBlock = currentBlock;
        Block *temp = currentBlock;

        bool isSymmetrical = true;
        Block *symmetryCheck = new Block();
        rotateBlockClockwise(temp->shape, symmetryCheck->shape);
        if (temp->shape != symmetryCheck->shape) {
            isSymmetrical = false;
        }
        if (isSymmetrical) {
            currentBlock->right_rotation = temp;
            currentBlock->left_rotation = temp;
        }
        if (!isSymmetrical) {
            Block *rotation = temp;
            do {
                auto *newRotation = new Block();
                rotateBlockClockwise(rotation->shape, newRotation->shape);
                rotation->right_rotation = newRotation;
                newRotation->left_rotation = rotation;
                rotation = newRotation;
            } while (rotation->shape != temp->shape);
            Block *x = rotation->left_rotation;
            x->right_rotation = temp;
            temp->left_rotation = x;
            delete rotation;

        }
        delete symmetryCheck;
    }

    Block *temp = initial_block;
    while (temp->next_block->next_block) {
        temp = temp->next_block;
    }
    power_up = temp->next_block->shape;
    Block *lastblock = temp->next_block;
    temp->next_block = nullptr;
    Block *rotation = lastblock->right_rotation;
    while (rotation != lastblock) {
        Block *nextRotation = rotation->right_rotation;
        rotation->left_rotation->right_rotation = rotation->right_rotation;
        rotation->right_rotation->left_rotation = rotation->left_rotation;
        delete rotation;
        rotation = nextRotation;
    }
    delete lastblock;
    inputFile.close();
}

void BlockFall::rotateBlockClockwise(const vector<vector<bool>> &src, vector<vector<bool>> &dst) {
    int rows = src.size();
    int cols = src[0].size();

    dst.resize(cols, vector<bool>(rows, false));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            dst[j][rows - 1 - i] = src[i][j];
        }
    }
}

void BlockFall::moveActiveBlockRight() {
    if (activeBlockCol < cols - 1) {
        // Check if the active block can move to the right within the grid bounds
        if (!isCollisionOnEntry(active_rotation, activeBlockRow, activeBlockCol + 1)) {
            // Update the active block's column position
            ++activeBlockCol;
        }
    }
}

void BlockFall::moveActiveBlockLeft() {
    if (activeBlockCol > 0) {
        // Check if the active block can move to the left within the grid bounds
        if (!isCollisionOnEntry(active_rotation, activeBlockRow, activeBlockCol - 1)) {
            // Update the active block's column position
            --activeBlockCol;
        }
    }
}

void BlockFall::rotateActiveBlockClockwise() {
    // Create a copy of the current active block rotation
    Block *originalRotation = active_rotation;

    // Move to the next rotation
    active_rotation = active_rotation->right_rotation;

    // Check if the rotation is valid (no collision and does not overflow)
    if (isCollisionOnEntry(active_rotation, activeBlockRow, activeBlockCol) ||
        activeBlockCol + active_rotation->shape[0].size() > cols) {
        // Restore the original rotation if the new rotation is not valid
        active_rotation = originalRotation;
    }
}

void BlockFall::rotateActiveBlockCounterClockwise() {
    // Create a copy of the current active block rotation
    Block *originalRotation = active_rotation;

    // Move to the previous rotation
    active_rotation = active_rotation->left_rotation;

    // Check if the rotation is valid (no collision and does not overflow)
    if (isCollisionOnEntry(active_rotation, activeBlockRow, activeBlockCol) ||
        activeBlockCol + active_rotation->shape[0].size() > cols) {
        // Restore the original rotation if the new rotation is not valid
        active_rotation = originalRotation;
    }
}

bool BlockFall::isCollisionOnEntry(const Block *block, int startRow, int startCol) const {
    // Check if there is a collision for the new block's entry into the grid

    // Iterate through the block's shape
    for (int i = 0; i < block->shape.size(); ++i) {
        for (int j = 0; j < block->shape[i].size(); ++j) {
            // Calculate the corresponding position on the grid
            int gridRow = startRow + i;
            int gridCol = startCol + j;

            // Check if the grid position is out of bounds
            if (gridRow < 0 || gridRow >= rows || gridCol < 0 || gridCol >= cols) {
                return true;  // Collision with grid boundaries
            }

            // Check if the grid position is already occupied
            if (grid[gridRow][gridCol] == 1 && block->shape[i][j] == 1) {
                return true;  // Collision with an occupied cell
            }
        }
    }

    return false;  // No collision detected
}


void BlockFall::printGrid() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 1) {
                cout << occupiedCellChar;
            } else {
                cout << unoccupiedCellChar;
            }
        }
        cout << endl;
    }
    cout << endl;
    cout << endl;
}

void BlockFall::dropActiveBlock() {
    int originalRow = activeBlockRow;
    // Drop the active block until it can no longer move down
    while (!isCollisionOnEntry(active_rotation, activeBlockRow + 1, activeBlockCol)) {
        activeBlockRow++;
    }
    int distanceFallen = activeBlockRow - originalRow;
    // Update the grid to reflect the dropped block

    int occupiedCell = 0;
    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
            if (active_rotation->shape[i][j]) {
                occupiedCell++;
            }
        }
    }
    current_score += distanceFallen * occupiedCell;
    updateGrid();
    activeBlockRow = 0;
    activeBlockCol = 0;

    // Move to the next block in the sequence
    if (active_rotation->next_block != nullptr) {
        active_rotation = active_rotation->next_block;
    } else {
        // No more blocks, end the game
        active_rotation = nullptr;
    }
}

void BlockFall::updateGrid() {
    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
            int gridRow = activeBlockRow + i;
            int gridCol = activeBlockCol + j;

            // Update the grid with the dropped block
            if (active_rotation->shape[i][j]) {
                grid[gridRow][gridCol] = 1;
            }
        }
    }
}

void BlockFall::toggleGravity() {
    if (!gravity_mode_on) {
        gravity_mode_on = true;
        for (int col = 0; col < cols; ++col) {
            // Iterate from bottom to top within each column
            for (int row = rows - 1; row >= 0; --row) {
                // Check if the current cell is occupied
                if (grid[row][col] == 1) {
                    // Move the occupied cell as far down as possible
                    int newRow = row;
                    while (newRow + 1 < rows && grid[newRow + 1][col] == 0) {
                        ++newRow;
                    }

                    // Check if the new position is within the grid bounds
                    if (newRow != row) {
                        // Update the grid to reflect the new position
                        grid[newRow][col] = 1;
                        grid[row][col] = 0;
                    }
                }
            }
        }
    }
    else {
        gravity_mode_on = false;
    }
}

void BlockFall::handleFullRows() {
    // Iterate through the rows to check for full rows
    vector<int> fullRows;
    bool temp = false;
    for (int i = 0; i < rows; ++i) {
        bool isFullRow = true;
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 0) {
                isFullRow = false;
                break;
            }
        }
        if (isFullRow) {
            fullRows.push_back(i);
            temp= true;
        }
    }
    if(temp){
        cout<<"Before clearing:"<<endl;
        printGrid();
        // Clear the marked full rows and update the score
        for (int i = fullRows.size() - 1; i >= 0; --i) {
            int rowIndex = fullRows[i];
            grid.erase(grid.begin() + rowIndex);
        }
        for (int i = 0; i < fullRows.size(); ++i) {
            grid.insert(grid.begin(), vector<int>(cols, 0));
        }

        // Update the score based on the number of cleared rows and columns
        int bonusPoints = cols * fullRows.size();
        current_score += bonusPoints;
    }
}

void BlockFall::handlePowerUp() {

    // Get the dimensions of the power-up shape
    int powerUpRows = power_up.size();
    int powerUpCols = power_up[0].size();

    // Iterate through every possible 5x5 part of the grid
    for (int i = 0; i <= rows - powerUpRows; ++i) {
        for (int j = 0; j <= cols - powerUpCols; ++j) {
            // Check if the current 5x5 part of the grid matches the power-up shape
            bool powerUpDetected = true;

            for (int pi = 0; pi < powerUpRows; ++pi) {
                for (int pj = 0; pj < powerUpCols; ++pj) {
                    if (power_up[pi][pj] != grid[i + pi][j + pj]) {
                        powerUpDetected = false;
                        break;
                    }
                }
                if (!powerUpDetected) {
                    break;
                }
            }

            // Activate power-up if recognized
            if (powerUpDetected) {
                cout << "Before clearing:" << endl;
                printGrid();

                // Award additional points for each cleared cell
                for (int i = 0; i < rows; ++i) {
                    for (int j = 0; j < cols; ++j) {
                        if (grid[i][j]) {
                            ++current_score;
                        }
                    }
                }
                // Clear the grid
                for (int i = 0; i < rows; ++i) {
                    fill(grid[i].begin(), grid[i].end(), 0);
                }
                // Update the score
                current_score += 1000;
            }
        }
    }
}

void BlockFall::specialHandleRowsForGravity(){vector<int> fullRows;
    bool temp = false;
    for (int i = 0; i < rows; ++i) {
        bool isFullRow = true;
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 0) {
                isFullRow = false;
                break;
            }
        }
        if (isFullRow) {
            fullRows.push_back(i);
            temp= true;
        }
    }
    if(temp){
        // Clear the marked full rows and update the score
        for (int i = fullRows.size() - 1; i >= 0; --i) {
            int rowIndex = fullRows[i];
            grid.erase(grid.begin() + rowIndex);
        }
        for (int i = 0; i < fullRows.size(); ++i) {
            grid.insert(grid.begin(), vector<int>(cols, 0));
        }

        // Update the score based on the number of cleared rows and columns
        int bonusPoints = cols * fullRows.size();
        current_score += bonusPoints;
    }

}