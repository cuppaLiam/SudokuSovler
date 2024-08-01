#include <stdio.h>
#include <stdbool.h>


int sudokuInput[9][9] =  {{8,0,0,0,0,0,0,0,0},
                          {0,0,3,6,0,0,0,0,0},
                          {0,7,0,0,9,0,2,0,0},
                          {0,5,0,0,0,7,0,0,0},
                          {0,0,0,0,4,5,7,0,0},
                          {0,0,0,1,0,0,0,3,0},
                          {0,0,1,0,0,0,0,6,8},
                          {0,0,8,5,0,0,0,1,0},
                          {0,9,0,0,0,0,4,0,0}};
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

    // Check for pairs, triples, and quads
    for (int size = 2; size <= 4; size++) {
        // Track which cells contain each possible value
        int valueLocations[9][9] = {0};  // [value][cell_index]
        int valueCounts[9] = {0};        // Count of cells for each value

        // Count occurrences of each possible value
        for (int i = 0; i < 9; i++) {
            if (unitCells[i].value == 0) {  // Only check unsolved cells
                for (int v = 0; v < 9; v++) {
                    if (unitCells[i].possibleValues[v]) {
                        valueLocations[v][valueCounts[v]++] = i;
                    }
                }
            }
        }

        // Check for hidden sets of the current size
        for (int v1 = 0; v1 < 9; v1++) {
            if (valueCounts[v1] == size) {
                int setValues[4] = {v1};
                int setCount = 1;
                int setCells[4];
                for (int i = 0; i < size; i++) {
                    setCells[i] = valueLocations[v1][i];
                }

                // Find other values that appear only in these cells
                for (int v2 = v1 + 1; v2 < 9; v2++) {
                    if (valueCounts[v2] == size) {
                        bool match = true;
                        for (int i = 0; i < size; i++) {
                            if (valueLocations[v2][i] != setCells[i]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                            setValues[setCount++] = v2;
                            if (setCount == size) {
                                break;
                            }
                        }
                    }
                }

                // If we found a hidden set
                if (setCount == size) {
                    // Remove other possible values from these cells
                    for (int i = 0; i < size; i++) {
                        Cell *cell = &unitCells[setCells[i]];
                        for (int v = 0; v < 9; v++) {
                            bool inSet = false;
                            for (int j = 0; j < size; j++) {
                                if (v == setValues[j]) {
                                    inSet = true;
                                    break;
                                }
                            }
                            if (!inSet && cell->possibleValues[v]) {
                                cell->possibleValues[v] = false;
                                updated = true;
                                printf("Removed %d from cell %d due to hidden %s\n",
                                       v + 1, setCells[i],
                                       size == 2 ? "pair" : (size == 3 ? "triplet" : "quad"));
                            }
                        }
                    }
                }
            }
        }
    }

    return updated;
}




bool boxLineReduction(Cell sudoku[9][9]) {
    bool updated = false;

    for (int boxRow = 0; boxRow < 3; boxRow++) {
        for (int boxCol = 0; boxCol < 3; boxCol++) {
            // Track possible values in each row and column of the box
            bool rowPossibleValues[3][9] = {false}; // 3 rows in the box
            bool colPossibleValues[3][9] = {false}; // 3 columns in the box

            // Check possible values for each cell in the box
            for (int subRow = 0; subRow < 3; subRow++) {
                for (int subCol = 0; subCol < 3; subCol++) {
                    Cell* cell = &sudoku[boxRow * 3 + subRow][boxCol * 3 + subCol];
                    if (cell->value == 0) { // Only consider cells with no value
                        for (int num = 0; num < 9; num++) {
                            if (cell->possibleValues[num]) {
                                rowPossibleValues[subRow][num] = true;
                                colPossibleValues[subCol][num] = true;
                            }
                        }
                    }
                }
            }

            // Check rows in the box
            for (int row = 0; row < 3; row++) {
                for (int num = 0; num < 9; num++) {
                    if (rowPossibleValues[row][num]) {
                        // Check if this number can only be in one row of the box
                        bool foundInOtherRows = false;
                        for (int otherRow = 0; otherRow < 3; otherRow++) {
                            if (otherRow != row && rowPossibleValues[otherRow][num]) {
                                foundInOtherRows = true;
                                break;
                            }
                        }
                        if (!foundInOtherRows) {
                            // Remove this number from all other cells in the same column
                            for (int col = 0; col < 9; col++) {
                                if (col / 3 != boxCol) {
                                    Cell* cell = &sudoku[boxRow * 3 + row][col];
                                    if (cell->possibleValues[num]) {
                                        cell->possibleValues[num] = false;
                                        printf("Removed %d from [%d][%d] due to Box Line Reduction in row %d of box [%d][%d]\n",
                                               num + 1, boxRow * 3 + row, col, row + 1, boxRow, boxCol);
                                        updated = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Check columns in the box
            for (int col = 0; col < 3; col++) {
                for (int num = 0; num < 9; num++) {
                    if (colPossibleValues[col][num]) {
                        // Check if this number can only be in one column of the box
                        bool foundInOtherCols = false;
                        for (int otherCol = 0; otherCol < 3; otherCol++) {
                            if (otherCol != col && colPossibleValues[otherCol][num]) {
                                foundInOtherCols = true;
                                break;
                            }
                        }
                        if (!foundInOtherCols) {
                            // Remove this number from all other cells in the same row
                            for (int row = 0; row < 9; row++) {
                                if (row / 3 != boxRow) {
                                    Cell* cell = &sudoku[row][boxCol * 3 + col];
                                    if (cell->possibleValues[num]) {
                                        cell->possibleValues[num] = false;
                                        printf("Removed %d from [%d][%d] due to Box Line Reduction in column %d of box [%d][%d]\n",
                                               num + 1, row, boxCol * 3 + col, col + 1, boxRow, boxCol);
                                        updated = true;
                                    }
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

bool advancedPointing(Cell sudoku[9][9]) {
    bool updated = false;

    // Iterate through each region (3x3 sub-grids)
    for (int regRow = 0; regRow < 3; regRow++) {
        for (int regCol = 0; regCol < 3; regCol++) {
            for (int posVal = 0; posVal < 9; posVal++) {
                int rowOccurrences[3] = {0, 0, 0};
                int colOccurrences[3] = {0, 0, 0};
                int totalOccurrences = 0;

                // Check occurrences of posVal in the sub-grid
                for (int subRow = 0; subRow < 3; subRow++) {
                    for (int subCol = 0; subCol < 3; subCol++) {
                        Cell* current_cell = &sudoku[regRow * 3 + subRow][regCol * 3 + subCol];
                        if (current_cell->value == 0 && current_cell->possibleValues[posVal]) {
                            rowOccurrences[subRow]++;
                            colOccurrences[subCol]++;
                            totalOccurrences++;
                        }
                    }
                }

                // Regular pointing (same row or column within a region)
                for (int i = 0; i < 3; i++) {
                    if (rowOccurrences[i] == totalOccurrences && totalOccurrences > 1) {
                        for (int col = 0; col < 9; col++) {
                            if (col / 3 != regCol) {
                                if (sudoku[regRow * 3 + i][col].possibleValues[posVal]) {
                                    printf("\nRemoving posVal %d from [%d][%d] due to regular pointing in row.", posVal + 1, regRow * 3 + i, col);
                                    sudoku[regRow * 3 + i][col].possibleValues[posVal] = false;
                                    updated = true;
                                }
                            }
                        }
                    }

                    if (colOccurrences[i] == totalOccurrences && totalOccurrences > 1) {
                        for (int row = 0; row < 9; row++) {
                            if (row / 3 != regRow) {
                                if (sudoku[row][regCol * 3 + i].possibleValues[posVal]) {
                                    printf("\nRemoving posVal %d from [%d][%d] due to regular pointing in column.", posVal + 1, row, regCol * 3 + i);
                                    sudoku[row][regCol * 3 + i].possibleValues[posVal] = false;
                                    updated = true;
                                }
                            }
                        }
                    }
                }

                // Perpendicular pointing (same two rows or columns within a region)
                if (totalOccurrences > 1) {
                    int rowPairs = 0;
                    int colPairs = 0;
                    for (int i = 0; i < 3; i++) {
                        if (rowOccurrences[i] > 0) rowPairs++;
                        if (colOccurrences[i] > 0) colPairs++;
                    }

                    if (rowPairs == 2) {
                        int rowsWithPosVal[3] = {-1, -1, -1};
                        int rowCount = 0;
                        for (int i = 0; i < 3; i++) {
                            if (rowOccurrences[i] > 0) rowsWithPosVal[rowCount++] = i;
                        }

                        for (int col = 0; col < 9; col++) {
                            if (col / 3 != regCol) {
                                bool remove = true;
                                for (int i = 0; i < rowCount; i++) {
                                    if (sudoku[regRow * 3 + rowsWithPosVal[i]][col].possibleValues[posVal]) {
                                        remove = false;
                                        break;
                                    }
                                }
                                if (remove) {
                                    for (int i = 0; i < rowCount; i++) {
                                        if (sudoku[regRow * 3 + rowsWithPosVal[i]][col].possibleValues[posVal]) {
                                            printf("\nRemoving posVal %d from [%d][%d] due to perpendicular pointing in rows.", posVal + 1, regRow * 3 + rowsWithPosVal[i], col);
                                            sudoku[regRow * 3 + rowsWithPosVal[i]][col].possibleValues[posVal] = false;
                                            updated = true;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (colPairs == 2) {
                        int colsWithPosVal[3] = {-1, -1, -1};
                        int colCount = 0;
                        for (int i = 0; i < 3; i++) {
                            if (colOccurrences[i] > 0) colsWithPosVal[colCount++] = i;
                        }

                        for (int row = 0; row < 9; row++) {
                            if (row / 3 != regRow) {
                                bool remove = true;
                                for (int i = 0; i < colCount; i++) {
                                    if (sudoku[row][regCol * 3 + colsWithPosVal[i]].possibleValues[posVal]) {
                                        remove = false;
                                        break;
                                    }
                                }
                                if (remove) {
                                    for (int i = 0; i < colCount; i++) {
                                        if (sudoku[row][regCol * 3 + colsWithPosVal[i]].possibleValues[posVal]) {
                                            printf("\nRemoving posVal %d from [%d][%d] due to perpendicular pointing in columns.", posVal + 1, row, regCol * 3 + colsWithPosVal[i]);
                                            sudoku[row][regCol * 3 + colsWithPosVal[i]].possibleValues[posVal] = false;
                                            updated = true;
                                        }
                                    }
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

        // Process hidden pairs/triplets/quads
        for (int i = 0; i < 9; i++) {
            if (hidPairs(sudoku[i])) update_made = true;      // Row

            Cell colCells[9];
            Cell regCells[9];
            for (int j = 0; j < 9; j++) {
                colCells[j] = sudoku[j][i];
                regCells[j] = sudoku[(i/3)*3 + j/3][(i%3)*3 + j%3];
            }
            if (hidPairs(colCells)) {
                update_made = true;
                for (int j = 0; j < 9; j++) sudoku[j][i] = colCells[j];
            }
            if (hidPairs(regCells)) {
                update_made = true;
                for (int j = 0; j < 9; j++) sudoku[(i/3)*3 + j/3][(i%3)*3 + j%3] = regCells[j];
            }
        }


        // Advanced pointing function
        /*
        if (advancedPointing(sudoku)) {
            update_made = true;
        }
         */

        if (boxLineReduction(sudoku)) {
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
