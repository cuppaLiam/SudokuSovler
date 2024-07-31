#include <stdio.h>
#include <stdbool.h>


int sudokuInput[9][9] =  {{3,0,0,1,0,0,8,0,5},
                          {0,0,0,9,0,0,7,2,0},
                          {0,0,6,0,0,0,0,0,0},
                          {0,0,0,0,0,0,0,0,8},
                          {0,2,0,4,8,7,0,0,0},
                          {0,7,0,0,0,1,0,0,0},
                          {2,3,0,0,0,0,0,0,0},
                          {0,0,5,0,0,9,0,4,0},
                          {4,0,0,0,0,0,2,0,1}};
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

    // Check for pairs, triples, and quads
    for (int size = 2; size <= 4; size++) {
        for (int i = 0; i < 9 - size + 1; i++) {
            if (unitCells[i].value != 0) continue;

            int possibleValues[9] = {0};
            int possibleCount = 0;

            // Count possible values for the first cell
            for (int v = 0; v < 9; v++) {
                if (unitCells[i].possibleValues[v]) {
                    possibleValues[possibleCount++] = v;
                }
            }

            if (possibleCount == size) {
                int matchCount = 1;
                int matchIndices[4] = {i};

                // Find cells with the same possible values
                for (int j = i + 1; j < 9; j++) {
                    if (unitCells[j].value != 0) continue;

                    bool isMatch = true;
                    int jPossibleCount = 0;
                    for (int v = 0; v < 9; v++) {
                        if (unitCells[j].possibleValues[v]) {
                            jPossibleCount++;
                            if (!unitCells[i].possibleValues[v]) {
                                isMatch = false;
                                break;
                            }
                        }
                    }

                    if (isMatch && jPossibleCount == size) {
                        matchIndices[matchCount++] = j;
                        if (matchCount == size) break;
                    }
                }

                // If we found a pair/triple/quad
                if (matchCount == size) {
                    // Remove these possible values from other cells
                    for (int j = 0; j < 9; j++) {
                        bool isMatch = false;
                        for (int k = 0; k < size; k++) {
                            if (j == matchIndices[k]) {
                                isMatch = true;
                                break;
                            }
                        }

                        if (!isMatch && unitCells[j].value == 0) {
                            for (int v = 0; v < possibleCount; v++) {
                                if (unitCells[j].possibleValues[possibleValues[v]]) {
                                    unitCells[j].possibleValues[possibleValues[v]] = false;
                                    updated = true;
                                }
                            }
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

        // Process obvious pairs/triplets/quads
        for (int i = 0; i < 9; i++) {
            if (obvPairs(sudoku[i])) update_made = true;  // Row

            Cell colCells[9];
            Cell regCells[9];
            for (int j = 0; j < 9; j++) {
                colCells[j] = sudoku[j][i];
                regCells[j] = sudoku[(i/3)*3 + j/3][(i%3)*3 + j%3];
            }
            if (obvPairs(colCells)) {
                update_made = true;
                for (int j = 0; j < 9; j++) sudoku[j][i] = colCells[j];
            }
            if (obvPairs(regCells)) {
                update_made = true;
                for (int j = 0; j < 9; j++) sudoku[(i/3)*3 + j/3][(i%3)*3 + j%3] = regCells[j];
            }
        }

        // Process hidden pairs/triplets
        for (int i = 0; i < 9; i++) {
            if (hidPairs(sudoku[i])) update_made = true;      // Row
            if (hidPairs(sudoku[i][0].colCells)) update_made = true; // Column
            if (hidPairs(sudoku[i][0].regCells)) update_made = true; // Region
        }

        /*
        // Advanced pointing function
        if (advancedPointing(sudoku)) {
            update_made = true;
        }
        if (boxLineReduction(sudoku)) {
            update_made = true;
        }
        if (xWing(sudoku)) {
            update_made = true;
        }
         */


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
