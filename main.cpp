#include <iostream>
using namespace std;


struct Cell {
    int row, col, value;
    Cell* nextRow;
    Cell* nextCol;

    Cell(int r, int c, int v) : row(r), col(c), value(v), nextRow(nullptr), nextCol(nullptr) {}
};


struct Header {
    int index;
    Cell* next; // puntero al primer nodo
    Header* nextHeader;

    Header(int i) : index(i), next(nullptr), nextHeader(nullptr) {}
};


class SparseMatrix {
private:
    Header* rowHead;
    Header* colHead;
    int rows, cols;

    // Buscar o crear cabecera para filas
    Header* getOrCreateRowHeader(int row) {
        Header* prev = nullptr;
        Header* curr = rowHead;

        while (curr && curr->index < row) {
            prev = curr;
            curr = curr->nextHeader;
        }

        if (!curr || curr->index != row) {
            Header* newHeader = new Header(row);
            if (prev) {
                prev->nextHeader = newHeader;
            } else {
                rowHead = newHeader;
            }
            newHeader->nextHeader = curr;
            return newHeader;
        }
        return curr;
    }

    // Buscar o crear cabecera para columnas
    Header* getOrCreateColHeader(int col) {
        Header* prev = nullptr;
        Header* curr = colHead;

        while (curr && curr->index < col) {
            prev = curr;
            curr = curr->nextHeader;
        }

        if (!curr || curr->index != col) {
            Header* newHeader = new Header(col);
            if (prev) {
                prev->nextHeader = newHeader;
            } else {
                colHead = newHeader;
            }
            newHeader->nextHeader = curr;
            return newHeader;
        }
        return curr;
    }

public:
    SparseMatrix(int r, int c) : rowHead(nullptr), colHead(nullptr), rows(r), cols(c) {}

    void insert(int row, int col, int value) {
        if (row < 0 || row >= rows || col < 0 || col >= cols || value == 0) return;

        Header* rowHeader = getOrCreateRowHeader(row);
        Header* colHeader = getOrCreateColHeader(col);

        Cell* newCell = new Cell(row, col, value);

        // Insertar en la fila
        Cell* rowPrev = nullptr;
        Cell* rowCurr = rowHeader->next;
        while (rowCurr && rowCurr->col < col) {
            rowPrev = rowCurr;
            rowCurr = rowCurr->nextRow;
        }
        if (rowPrev) {
            rowPrev->nextRow = newCell;
        } else {
            rowHeader->next = newCell;
        }
        newCell->nextRow = rowCurr;

        // Insertar en la columna
        Cell* colPrev = nullptr;
        Cell* colCurr = colHeader->next;
        while (colCurr && colCurr->row < row) {
            colPrev = colCurr;
            colCurr = colCurr->nextCol;
        }
        if (colPrev) {
            colPrev->nextCol = newCell;
        } else {
            colHeader->next = newCell;
        }
        newCell->nextCol = colCurr;
    }

    // Mostrar la matriz
    void display() {
        for (int i = 0; i < rows; ++i) {
            Header* rowHeader = getOrCreateRowHeader(i);
            Cell* cell = rowHeader->next;
            for (int j = 0; j < cols; ++j) {
                if (cell && cell->col == j) {
                    cout << cell->value << " ";
                    cell = cell->nextRow;
                } else {
                    cout << "0 ";
                }
            }
            cout << endl;
        }
    }

    ~SparseMatrix() {
        Header* rowCurr = rowHead;
        while (rowCurr) {
            Cell* cellCurr = rowCurr->next;
            while (cellCurr) {
                Cell* toDelete = cellCurr;
                cellCurr = cellCurr->nextRow;
                delete toDelete;
            }
            Header* toDelete = rowCurr;
            rowCurr = rowCurr->nextHeader;
            delete toDelete;
        }

        Header* colCurr = colHead;
        while (colCurr) {
            Header* toDelete = colCurr;
            colCurr = colCurr->nextHeader;
            delete toDelete;
        }
    }
    void remove(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return;

        Header* rowHeader = getOrCreateRowHeader(row);
        Header* colHeader = getOrCreateColHeader(col);

        // Eliminar de la fila
        Cell* rowPrev = nullptr;
        Cell* rowCurr = rowHeader->next;
        while (rowCurr && rowCurr->col != col) {
            rowPrev = rowCurr;
            rowCurr = rowCurr->nextRow;
        }
        if (rowCurr) {
            if (rowPrev) rowPrev->nextRow = rowCurr->nextRow;
            else rowHeader->next = rowCurr->nextRow;
        }

        // Eliminar de la columna
        Cell* colPrev = nullptr;
        Cell* colCurr = colHeader->next;
        while (colCurr && colCurr->row != row) {
            colPrev = colCurr;
            colCurr = colCurr->nextCol;
        }
        if (colCurr) {
            if (colPrev) colPrev->nextCol = colCurr->nextCol;
            else colHeader->next = colCurr->nextCol;
        }

        delete rowCurr;
    }

    int search(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return 0;

        Header* rowHeader = getOrCreateRowHeader(row);
        Cell* cell = rowHeader->next;
        while (cell) {
            if (cell->col == col) return cell->value;
            cell = cell->nextRow;
        }
        return 0; // No encontrado
    }

    int operator()(int row, int col) {
        return search(row, col);
    }

    bool compare(SparseMatrix& other) {
        if (rows != other.rows || cols != other.cols) return false;

        Header* rowCurr = rowHead;
        while (rowCurr) {
            Cell* cellCurr = rowCurr->next;
            while (cellCurr) {
                if (cellCurr->value != other.search(cellCurr->row, cellCurr->col)) return false;
                cellCurr = cellCurr->nextRow;
            }
            rowCurr = rowCurr->nextHeader;
        }
        return true;
    }

    SparseMatrix add(SparseMatrix& other) {
        SparseMatrix result(rows, cols);

        for (int i = 0; i < rows; ++i) {
            Header* rowHeader = getOrCreateRowHeader(i);
            Cell* cell = rowHeader->next;
            while (cell) {
                int value = cell->value + other.search(cell->row, cell->col);
                result.insert(cell->row, cell->col, value);
                cell = cell->nextRow;
            }
        }
        return result;
    }

    SparseMatrix transpose() {
        SparseMatrix result(cols, rows);

        Header* rowCurr = rowHead;
        while (rowCurr) {
            Cell* cellCurr = rowCurr->next;
            while (cellCurr) {
                result.insert(cellCurr->col, cellCurr->row, cellCurr->value);
                cellCurr = cellCurr->nextRow;
            }
            rowCurr = rowCurr->nextHeader;
        }
        return result;
    }

    SparseMatrix multiply(SparseMatrix& other) {
        SparseMatrix result(rows, other.cols);

        for (int i = 0; i < rows; ++i) {
            Header* rowHeader = getOrCreateRowHeader(i);
            Cell* cellA = rowHeader->next;
            while (cellA) {
                for (int j = 0; j < other.cols; ++j) {
                    int value = search(cellA->row, cellA->col) * other.search(cellA->col, j);
                    if (value != 0) result.insert(cellA->row, j, value);
                }
                cellA = cellA->nextRow;
            }
        }
        return result;
    }

    SparseMatrix inverse() {
        if (rows != cols) {
            throw invalid_argument("La matriz debe ser cuadrada para calcular su inversa");
        }

        int n = rows; // La matriz es cuadrada, por lo que rows == cols
        SparseMatrix adj(n, n); // Matriz adjunta
        SparseMatrix result(n, n); // Matriz inversa

        // Calcular el determinante
        int det = determinant(*this);
        if (det == 0) {
            throw runtime_error("La matriz no tiene inversa (determinante es 0)");
        }

        // Calcular la adjunta
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                SparseMatrix minorMatrix = getMinor(i, j);
                int minorDet = determinant(minorMatrix);
                int cofactor = ((i + j) % 2 == 0 ? 1 : -1) * minorDet;

                adj.insert(j, i, cofactor); // Nota: transponer al llenar la adjunta
            }
        }

        // Calcular la inversa dividiendo la adjunta por el determinante
        for (int i = 0; i < n; ++i) {
            Header* rowHeader = adj.getOrCreateRowHeader(i);
            Cell* cell = rowHeader->next;
            while (cell) {
                result.insert(cell->row, cell->col, cell->value / det);
                cell = cell->nextRow;
            }
        }

        return result;
    }

    int determinant(SparseMatrix& matrix) {
        if (matrix.rows != matrix.cols) {
            throw invalid_argument("La matriz debe ser cuadrada para calcular el determinante");
        }

        int n = matrix.rows;

        // Caso base: matriz de 1x1
        if (n == 1) {
            return matrix.search(0, 0);
        }

        // Caso base: matriz de 2x2
        if (n == 2) {
            return matrix.search(0, 0) * matrix.search(1, 1) - matrix.search(0, 1) * matrix.search(1, 0);
        }

        // Caso general: expansión por la primera fila
        int det = 0;
        for (int j = 0; j < n; ++j) {
            SparseMatrix minorMatrix = matrix.getMinor(0, j);
            int cofactor = ((j % 2 == 0) ? 1 : -1) * matrix.search(0, j) * determinant(minorMatrix);
            det += cofactor;
        }

        return det;
    }

    SparseMatrix getMinor(int row, int col) {
        SparseMatrix minor(rows - 1, cols - 1);

        int minorRow = 0;
        for (int i = 0; i < rows; ++i) {
            if (i == row) continue;
            int minorCol = 0;
            for (int j = 0; j < cols; ++j) {
                if (j == col) continue;
                minor.insert(minorRow, minorCol, search(i, j));
                minorCol++;
            }
            minorRow++;
        }

        return minor;
    }

};


int main() {
    SparseMatrix sm(5, 5);

    sm.insert(1, 1, 10);
    sm.insert(2, 3, 20);
    sm.insert(4, 0, 30);

    cout << "Matriz dispersa:" << endl;
    sm.display();

    sm.insert(3, 2, 8);

    cout << "Matriz dispersa:" << endl;
    sm.display();

    sm.insert(3, 2, 80);

    cout << "Matriz dispersa:" << endl;
    sm.display();

    return 0;
}