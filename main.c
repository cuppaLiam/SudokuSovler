#include <stdio.h>
#include <stdbool.h>


int sudokuInput[9][9] =  {{0,0,3,7,2,0,1,0,6},
                          {0,0,0,0,0,0,5,3,0},
                          {1,0,0,0,0,0,7,0,0},
                          {7,0,8,6,0,0,0,0,0},
                          {4,0,0,0,0,0,0,0,1},
                          {0,6,0,0,0,8,0,0,0},
                          {0,0,5,0,8,4,3,0,0},
                          {0,0,0,0,1,0,0,8,0},
                          {0,0,0,0,0,5,0,0,2}};

typedef struct Cell {
    int value;
    bool possibleValues[9];
    int row;
    int col;
    struct Cell* rowCells[9];
    struct Cell* colCells[9];
    struct Cell* regCells[9];
} Cell;

void initSudoku(Cell sudoku[9][9]) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            Cell* current_cell = &sudoku[row][col];
            current_cell->row = row;
            current_cell->col = col;
            if (sudokuInput[row][col] != 0) {
                current_cell->value = sudokuInput[row][col];
                for (int i = 0; i < 9; i++) {
                    current_cell->possibleValues[i] = false; // No possibilities if value is set
                }
            } else {
                current_cell->value = 0;
                for (int i = 0; i < 9; i++) {
                    current_cell->possibleValues[i] = true; // All values possible initially
                }
            }
            for (int count = 0; count < 9; count++) {
                current_cell->rowCells[count] = &sudoku[row][count];
                current_cell->colCells[count] = &sudoku[count][col];
            }
            int regionRow = (row / 3) * 3;
            int regionCol = (col / 3) * 3;
            int regIndex = 0;
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    current_cell->regCells[regIndex++] = &sudoku[regionRow + r][regionCol + c];
                }
            }
        }
    }
}

void updatePossibleValues(Cell *current_cell){
    bool* possibleValues = current_cell->possibleValues;
    for (int count = 0; count < 9; count++){
        Cell* rowCell = current_cell->rowCells[count];
        Cell* colCell = current_cell->colCells[count];
        Cell* regCell = current_cell->regCells[count];
        if(rowCell->value != 0){
            possibleValues[rowCell->value - 1] = false;
        }
        if(colCell->value != 0){
            possibleValues[colCell->value - 1] = false;
        }
        if(regCell->value != 0){
            possibleValues[regCell->value - 1] = false;
        }
    }
}

int updateValue(Cell* current_cell) {
    int possibleCount = 0;
    int lastPossibleValue = 0;

    // Count possible values
    for (int i = 0; i < 9; i++) {
        if (current_cell->possibleValues[i]) {
            possibleCount++;
            lastPossibleValue = i + 1;
        }
    }

    // If there's only one possible value, set it
    if (possibleCount == 1) {
        current_cell->value = lastPossibleValue;
        return lastPossibleValue;
    }

    // Check if the current cell is the only one in its row with a possible value
    for (int posVal = 0; posVal < 9; posVal++) {
        if (current_cell->possibleValues[posVal]) {
            int count = 0;
            for (int i = 0; i < 9; i++) {
                if (current_cell->rowCells[i]->possibleValues[posVal]) {
                    count++;
                }
            }
            if (count == 1) {
                current_cell->value = posVal + 1;
                return posVal + 1;
            }
        }
    }

    // Check if the current cell is the only one in its column with a possible value
    for (int posVal = 0; posVal < 9; posVal++) {
        if (current_cell->possibleValues[posVal]) {
            int count = 0;
            for (int i = 0; i < 9; i++) {
                if (current_cell->colCells[i]->possibleValues[posVal]) {
                    count++;
                }
            }
            if (count == 1) {
                current_cell->value = posVal + 1;
                return posVal + 1;
            }
        }
    }

    // Check if the current cell is the only one in its region with a possible value
    for (int posVal = 0; posVal < 9; posVal++) {
        if (current_cell->possibleValues[posVal]) {
            int count = 0;
            for (int i = 0; i < 9; i++) {
                if (current_cell->regCells[i]->possibleValues[posVal]) {
                    count++;
                }
            }
            if (count == 1) {
                current_cell->value = posVal + 1;
                return posVal + 1;
            }
        }
    }

    // If no unique value found
    return 0;
}


void displaySudoku(Cell sudoku[9][9]){
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            printf("%d ", sudoku[row][col].value);
        }
        printf("\n");
    }
}

void displayPossibleValues(Cell sudoku[9][9]){
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (sudoku[row][col].value != 0){
                printf("[]");
            } else{
                printf("[");
                for (int posVal = 0; posVal <= 8; posVal++){
                    if (sudoku[row][col].possibleValues[posVal]){
                        printf("%d,", posVal + 1);
                    }
                }
                printf("]");
            }
        }
        printf("\n");
    }
}

bool obvPairs(Cell unitCells[9]) {
    bool updated = false;
    int occurrences[9] = {0};

    // Count occurrences of each possible value in the unit
    for (int count = 0; count < 9; count++) {
        Cell* current_cell = &unitCells[count];
        for (int posVal = 0; posVal < 9; posVal++) {
            if (current_cell->possibleValues[posVal]) {
                occurrences[posVal]++;
            }
        }
    }

    // Identify and process pairs and triplets
    for (int posVal = 0; posVal < 9; posVal++) {
        if (occurrences[posVal] == 2 || occurrences[posVal] == 3) {
            Cell* cells[3] = {NULL, NULL, NULL};
            int cellCount = 0;

            // Collect cells with the current possible value
            for (int count = 0; count < 9; count++) {
                if (unitCells[count].possibleValues[posVal]) {
                    cells[cellCount++] = &unitCells[count];
                }
            }

            // Check if all collected cells have the same possible values
            bool samePossibilities = true;
            for (int i = 1; i < cellCount; i++) {
                for (int j = 0; j < 9; j++) {
                    if (cells[0]->possibleValues[j] != cells[i]->possibleValues[j]) {
                        samePossibilities = false;
                        break;
                    }
                }
                if (!samePossibilities) break;
            }

            if (samePossibilities) {
                // Eliminate these values from other cells in the unit
                for (int count = 0; count < 9; count++) {
                    if (&unitCells[count] != cells[0] && &unitCells[count] != cells[1] && (cellCount == 2 || &unitCells[count] != cells[2])) {
                        if (unitCells[count].possibleValues[posVal]) {
                            unitCells[count].possibleValues[posVal] = false;
                            updated = true;
                        }
                    }
                }
            }
        }
    }

    return updated;
}

bool hidPairs(Cell unitCells[9]) {
    bool updated = false;

    for (int first = 0; first < 8; first++) {
        for (int second = first + 1; second < 9; second++) {
            int count = 0;
            Cell *pairCells[2] = {NULL, NULL};
            int pairCount = 0;

            // Find cells where only 'first' and 'second' are possible
            for (int cellIndex = 0; cellIndex < 9; cellIndex++) {
                Cell *cell = &unitCells[cellIndex];
                if (cell->possibleValues[first] && cell->possibleValues[second]) {
                    // Check that these are the only possibilities for this cell
                    bool validPair = true;
                    for (int posVal = 0; posVal < 9; posVal++) {
                        if (posVal != first && posVal != second && cell->possibleValues[posVal]) {
                            validPair = false;
                            break;
                        }
                    }
                    if (validPair) {
                        if (pairCount < 2) {
                            pairCells[pairCount++] = cell;
                        }
                        count++;
                    }
                }
            }

            if (count == 2 && pairCount == 2) {
                // We found exactly two cells with this hidden pair, clear other values
                for (int posVal = 0; posVal < 9; posVal++) {
                    if (posVal != first && posVal != second) {
                        if (pairCells[0]->possibleValues[posVal] || pairCells[1]->possibleValues[posVal]) {
                            pairCells[0]->possibleValues[posVal] = false;
                            pairCells[1]->possibleValues[posVal] = false;
                            updated = true;
                        }
                    }
                }
            }
        }
    }

    return updated;
}




bool pointing(Cell sudoku[9][9]) {
    printf("\npointing function called");
    bool updated = false;

    for (int regRow = 0; regRow < 3; regRow++) {
        for (int regCol = 0; regCol < 3; regCol++) {
            // For each region
            for (int posVal = 0; posVal < 9; posVal++) {
                int rowOccurrences[3] = {0, 0, 0};
                int colOccurrences[3] = {0, 0, 0};

                // Check occurrences of posVal in the sub-grid
                for (int subRow = 0; subRow < 3; subRow++) {
                    for (int subCol = 0; subCol < 3; subCol++) {
                        Cell current_cell = sudoku[(regRow * 3) + subRow][(regCol * 3) + subCol];
                        if (current_cell.value == 0 && current_cell.possibleValues[posVal]) {
                            rowOccurrences[subRow]++;
                            colOccurrences[subCol]++;
                        }
                    }
                }

                // Eliminate possible values in rows
                for (int subRow = 0; subRow < 3; subRow++) {
                    if (rowOccurrences[subRow] > 0 && (rowOccurrences[(subRow + 1) % 3] == 0 && rowOccurrences[(subRow + 2) % 3] == 0)) {
                        int row = (regRow * 3) + subRow;
                        for (int col = 0; col < 9; col++) {
                            if (col / 3 != regCol && sudoku[row][col].possibleValues[posVal]) {
                                printf("\nUpdating possibleValues[%d] for cell [%d][%d]", posVal, row, col);
                                sudoku[row][col].possibleValues[posVal] = false;
                                updated = true;
                            }
                        }
                    }
                }

                // Eliminate possible values in columns
                for (int subCol = 0; subCol < 3; subCol++) {
                    if (colOccurrences[subCol] > 0 && (colOccurrences[(subCol + 1) % 3] == 0 && colOccurrences[(subCol + 2) % 3] == 0)) {
                        int col = (regCol * 3) + subCol;
                        for (int row = 0; row < 9; row++) {
                            if (row / 3 != regRow && sudoku[row][col].possibleValues[posVal]) {
                                printf("\nUpdating possibleValues[%d] for cell [%d][%d]", posVal, row, col);
                                sudoku[row][col].possibleValues[posVal] = false;
                                updated = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return updated;
}

void solve(Cell sudoku[9][9]) {
    bool update_made;
    int itr = 0;

    do {
        printf("\nIteration: %d\n", itr++);
        displaySudoku(sudoku);
        displayPossibleValues(sudoku);

        update_made = false;

        // Process cell updates
        bool cell_solved;
        do {
            cell_solved = false;

            for (int row = 0; row < 9; row++) {
                for (int col = 0; col < 9; col++) {
                    Cell* current_cell = &sudoku[row][col];
                    if (current_cell->value == 0) {
                        updatePossibleValues(current_cell);
                        int value = updateValue(current_cell);
                        if (value != 0) {
                            printf("\nCell [%d][%d] value updated to %d", row, col, value);
                            cell_solved = true;
                            update_made = true;
                        }
                    }
                }
            }
        } while (cell_solved);

        // Process obvious pairs/triplets
        for (int i = 0; i < 9; i++) {
            if (obvPairs(sudoku[i])) update_made = true;      // Row
            if (obvPairs(sudoku[i][0].colCells)) update_made = true; // Column
            if (obvPairs(sudoku[i][0].regCells)) update_made = true; // Region
        }

        // Process hidden pairs/triplets
        for (int i = 0; i < 9; i++) {
            if (hidPairs(sudoku[i])) update_made = true;      // Row
            if (hidPairs(sudoku[i][0].colCells)) update_made = true; // Column
            if (hidPairs(sudoku[i][0].regCells)) update_made = true; // Region
        }


        //?
        if (!update_made) {
            break;
        }


        // Process pointing function
        bool pointing_updated = pointing(sudoku);
        if (pointing_updated) {
            update_made = true;
        }
        

        printf("\nUpdate made: %s", update_made ? "true" : "false");

    } while (update_made);
}



int main(){
    Cell sudoku[9][9];
    initSudoku(sudoku);
    printf("initial sudoku\n");
    displaySudoku(sudoku);

    solve(sudoku);
    printf("\nfinal sudoku\n");
    displaySudoku(sudoku);
    displayPossibleValues(sudoku);
    return 0;
}
