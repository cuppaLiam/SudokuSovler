#include <stdio.h>
#include <stdbool.h>


int sudokuInput[9][9] =  {{5,0,0,0,1,0,0,7,0},
                          {7,0,0,5,0,0,0,0,0},
                          {0,0,3,6,0,0,4,0,0},
                          {0,0,0,0,5,2,0,0,0},
                          {8,0,0,3,0,0,0,0,0},
                          {4,9,0,0,0,0,0,0,7},
                          {0,0,0,0,0,0,0,0,0},
                          {0,0,1,0,2,3,0,5,0},
                          {0,4,0,1,0,0,0,0,8}};

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

y
/*
bool boxLineReduction(Cell sudoku[9][9], int row, int col) {
    bool updated = false;
    int regionStartRow = (row / 3) * 3;
    int regionStartCol = (col / 3) * 3;

    // Checking row for box-line reduction
    for (int num = 0; num < 9; num++) {
        int occurrences = 0;
        bool rowHasNum = false;
        bool regionHasNum = false;
        int colIndex = -1;

        // Check if the number can only exist in a single row of the region
        for (int c = 0; c < 9; c++) {
            Cell* current_cell = &sudoku[row][c];
            if (current_cell->possibleValues[num]) {
                occurrences++;
                colIndex = c;
            }
            if (current_cell->value == num + 1) {
                rowHasNum = true;
                break;
            }
        }

        if (!rowHasNum && occurrences > 0) {
            for (int r = regionStartRow; r < regionStartRow + 3; r++) {
                for (int c = regionStartCol; c < regionStartCol + 3; c++) {
                    if (r != row && sudoku[r][c].possibleValues[num]) {
                        regionHasNum = true;
                        break;
                    }
                }
                if (regionHasNum) break;
            }
        }

        if (!regionHasNum && occurrences == 1) {
            for (int c = 0; c < 9; c++) {
                if (sudoku[row][c].possibleValues[num]) {
                    sudoku[row][c].possibleValues[num] = false;
                    updated = true;
                }
            }
            if (colIndex != -1) {
                sudoku[row][colIndex].possibleValues[num] = true; // Restore the valid possibility
            }
        }
    }

    // Checking column for box-line reduction
    for (int num = 0; num < 9; num++) {
        int occurrences = 0;
        bool colHasNum = false;
        bool regionHasNum = false;
        int rowIndex = -1;

        // Check if the number can only exist in a single column of the region
        for (int r = 0; r < 9; r++) {
            Cell* current_cell = &sudoku[r][col];
            if (current_cell->possibleValues[num]) {
                occurrences++;
                rowIndex = r;
            }
            if (current_cell->value == num + 1) {
                colHasNum = true;
                break;
            }
        }

        if (!colHasNum && occurrences > 0) {
            for (int r = regionStartRow; r < regionStartRow + 3; r++) {
                for (int c = regionStartCol; c < regionStartCol + 3; c++) {
                    if (c != col && sudoku[r][c].possibleValues[num]) {
                        regionHasNum = true;
                        break;
                    }
                }
                if (regionHasNum) break;
            }
        }

        if (!regionHasNum && occurrences == 1) {
            for (int r = 0; r < 9; r++) {
                if (sudoku[r][col].possibleValues[num]) {
                    sudoku[r][col].possibleValues[num] = false;
                    updated = true;
                }
            }
            if (rowIndex != -1) {
                sudoku[rowIndex][col].possibleValues[num] = true; // Restore the valid possibility
            }
        }
    }

    return updated;
}

 */

// X-Wing implementation
bool xWing(Cell sudoku[9][9]) {
    bool updated = false;

    for (int num = 0; num < 9; num++) {
        for (int row1 = 0; row1 < 8; row1++) {
            for (int row2 = row1 + 1; row2 < 9; row2++) {
                int col1 = -1, col2 = -1;
                for (int col = 0; col < 9; col++) {
                    if (sudoku[row1][col].possibleValues[num] && sudoku[row2][col].possibleValues[num]) {
                        if (col1 == -1) col1 = col;
                        else if (col2 == -1) col2 = col;
                        else {
                            col1 = col2 = -1; // More than two columns
                            break;
                        }
                    }
                }
                if (col1 != -1 && col2 != -1) {
                    for (int row = 0; row < 9; row++) {
                        if (row != row1 && row != row2) {
                            if (sudoku[row][col1].possibleValues[num]) {
                                sudoku[row][col1].possibleValues[num] = false;
                                updated = true;
                            }
                            if (sudoku[row][col2].possibleValues[num]) {
                                sudoku[row][col2].possibleValues[num] = false;
                                updated = true;
                            }
                        }
                    }
                }
            }
        }

        for (int col1 = 0; col1 < 8; col1++) {
            for (int col2 = col1 + 1; col2 < 9; col2++) {
                int row1 = -1, row2 = -1;
                for (int row = 0; row < 9; row++) {
                    if (sudoku[row][col1].possibleValues[num] && sudoku[row][col2].possibleValues[num]) {
                        if (row1 == -1) row1 = row;
                        else if (row2 == -1) row2 = row;
                        else {
                            row1 = row2 = -1; // More than two rows
                            break;
                        }
                    }
                }
                if (row1 != -1 && row2 != -1) {
                    for (int col = 0; col < 9; col++) {
                        if (col != col1 && col != col2) {
                            if (sudoku[row1][col].possibleValues[num]) {
                                sudoku[row1][col].possibleValues[num] = false;
                                updated = true;
                            }
                            if (sudoku[row2][col].possibleValues[num]) {
                                sudoku[row2][col].possibleValues[num] = false;
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

// Y-Wing implementation
bool yWing(Cell sudoku[9][9]) {
    bool updated = false;

    for (int row1 = 0; row1 < 9; row1++) {
        for (int col1 = 0; col1 < 9; col1++) {
            if (sudoku[row1][col1].value == 0) {
                int possible1 = -1, possible2 = -1;
                for (int num = 0; num < 9; num++) {
                    if (sudoku[row1][col1].possibleValues[num]) {
                        if (possible1 == -1) possible1 = num;
                        else {
                            possible2 = num;
                            break;
                        }
                    }
                }
                if (possible2 == -1) continue;

                for (int row2 = 0; row2 < 9; row2++) {
                    if (row2 != row1) {
                        for (int col2 = 0; col2 < 9; col2++) {
                            if (sudoku[row2][col2].value == 0 && (row2 == row1 || col2 == col1)) {
                                int possible3 = -1, possible4 = -1;
                                for (int num = 0; num < 9; num++) {
                                    if (sudoku[row2][col2].possibleValues[num]) {
                                        if (possible3 == -1) possible3 = num;
                                        else {
                                            possible4 = num;
                                            break;
                                        }
                                    }
                                }
                                if (possible4 == -1) continue;

                                if ((possible1 == possible3 || possible1 == possible4) && (possible2 == possible3 || possible2 == possible4)) {
                                    for (int row3 = 0; row3 < 9; row3++) {
                                        if (row3 != row1 && row3 != row2) {
                                            if (sudoku[row3][col1].possibleValues[possible1] && sudoku[row3][col1].possibleValues[possible2]) {
                                                sudoku[row3][col1].possibleValues[possible1] = false;
                                                sudoku[row3][col1].possibleValues[possible2] = false;
                                                updated = true;
                                            }
                                            if (sudoku[row3][col2].possibleValues[possible1] && sudoku[row3][col2].possibleValues[possible2]) {
                                                sudoku[row3][col2].possibleValues[possible1] = false;
                                                sudoku[row3][col2].possibleValues[possible2] = false;
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
        if (yWing(sudoku)) {
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
