#include <iostream>
using namespace std;

// Function to input a sparse matrix
void inputSparseMatrix(int rows, int cols, int matrix[50][50]) {
    cout << "Enter the elements of the sparse matrix:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cin >> matrix[i][j];
        }
    }
}

// Function to display the entered matrix
void displayMatrix(int rows, int cols, int matrix[50][50]) {
    cout << "Here is the sparse matrix you entered:" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

// Function to count the non-zero elements in the matrix
int countNonZeroElements(int rows, int cols, int matrix[50][50]) {
    int nonZeroCount = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] != 0) {
                nonZeroCount++;
            }
        }
    }
    return nonZeroCount;
}

// Function to convert the sparse matrix to a 2D representation
void createSparseRepresentation(int rows, int cols, int matrix[50][50], int nonZeroCount, int sparse[3][50]) {
    int index = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] != 0) {
                sparse[0][index] = i;  // Row index
                sparse[1][index] = j;  // Column index
                sparse[2][index] = matrix[i][j];  // Value
                index++;
            }
        }
    }
}

// Function to display the sparse matrix representation
void displaySparseRepresentation(int nonZeroCount, int sparse[3][50]) {
    cout << "Sparse Matrix Representation:" << endl;
    cout << "Row ->    ";
    for (int i = 0; i < nonZeroCount; i++) {
        cout << sparse[0][i] << " ";
    }
    cout << endl;
    cout << "Column -> ";
    for (int i = 0; i < nonZeroCount; i++) {
        cout << sparse[1][i] << " ";
    }
    cout << endl;
    cout << "Value ->  ";
    for (int i = 0; i < nonZeroCount; i++) {
        cout << sparse[2][i] << " ";
    }
    cout << endl;
}

// Main function
int main() {
    int rows, cols;
    cout << "Enter the number of rows and columns of the matrix: ";
    cin >> rows >> cols;

    int matrix[50][50];  // Input matrix
    int sparse[3][50];   // Sparse matrix representation

    // Input and process the matrix
    inputSparseMatrix(rows, cols, matrix);
    displayMatrix(rows, cols, matrix);

    int nonZeroCount = countNonZeroElements(rows, cols, matrix);
    createSparseRepresentation(rows, cols, matrix, nonZeroCount, sparse);
    displaySparseRepresentation(nonZeroCount, sparse);

    return 0;
}
