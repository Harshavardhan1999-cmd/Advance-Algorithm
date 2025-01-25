#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int row;
    int col;
    int value;
    Node* next;

    Node(int r, int c, int v) : row(r), col(c), value(v), next(nullptr) {}
};

class SparseMatrix {
private:
    Node* head;

public:
    SparseMatrix() : head(nullptr) {}

    void addElement(int row, int col, int value) {
        Node* newNode = new Node(row, col, value);
        if (!head) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void display() {
        Node* temp = head;
        while (temp) {
            cout << "(" << temp->row << ", " << temp->col << ", " << temp->value << ") -> ";
            temp = temp->next;
        }
        cout << "NULL" << endl;
    }

    ~SparseMatrix() {
        Node* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }
};

void convertToLinkedList(const vector<vector<int>>& matrix, SparseMatrix& sparseList) {
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[i].size(); ++j) {
            if (matrix[i][j] != 0) {
                sparseList.addElement(i, j, matrix[i][j]);
            }
        }
    }
}

int main() {
    int rows, cols;
    cout << "Enter the number of rows and columns for the sparse matrix: ";
    cin >> rows >> cols;

    vector<vector<int>> matrix(rows, vector<int>(cols));

    cout << "Enter the matrix elements: " << endl;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cin >> matrix[i][j];
        }
    }

    cout << "Sparse matrix representation:" << endl;
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }

    SparseMatrix sparseList;
    convertToLinkedList(matrix, sparseList);

    cout << "Linked list representation of the sparse matrix:" << endl;
    sparseList.display();

    return 0;
}
