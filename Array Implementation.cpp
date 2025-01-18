#include <iostream>
#include <vector>
using namespace std;

// Function to read a matrix from user input
void readMatrix(int r, int c, vector<vector<int>> &mat) {
    cout << "Enter the elements of the matrix:" << endl;
    for (int i = 0; i < r; i++) {
        vector<int> row(c);
        for (int j = 0; j < c; j++) {
            cin >> row[j];
        }
        mat.push_back(row);
    }
}

// Function to display a matrix
void printMatrix(const vector<vector<int>> &mat) {
    cout << "Here is the matrix you entered:" << endl;
    for (const auto &row : mat) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

// Function to generate sparse matrix representation
vector<vector<int>> generateSparse(const vector<vector<int>> &mat) {
    vector<vector<int>> sparse(3);
    for (int i = 0; i < mat.size(); i++) {
        for (int j = 0; j < mat[i].size(); j++) {
            if (mat[i][j] != 0) {
                sparse[0].push_back(i);  // Row index
                sparse[1].push_back(j);  // Column index
                sparse[2].push_back(mat[i][j]);  // Non-zero value
            }
        }
    }
    return sparse;
}

// Function to display sparse matrix representation
void printSparse(const vector<vector<int>> &sparse) {
    cout << "Sparse Matrix Representation:" << endl;
    cout << "Row ->    ";
    for (int val : sparse[0]) cout << val << " ";
    cout << endl;
    cout << "Column -> ";
    for (int val : sparse[1]) cout << val << " ";
    cout << endl;
    cout << "Value ->  ";
    for (int val : sparse[2]) cout << val << " ";
    cout << endl;
}

int main() {
    int r, c;
    cout << "Enter rows and columns of the matrix: ";
    cin >> r >> c;

    vector<vector<int>> mat;  // Input matrix
    readMatrix(r, c, mat);
    printMatrix(mat);

    vector<vector<int>> sparse = generateSparse(mat);
    printSparse(sparse);

    return 0;
}
