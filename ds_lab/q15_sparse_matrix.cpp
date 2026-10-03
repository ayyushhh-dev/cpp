// Q15: Convert a Sparse Matrix into non-zero (triplet) form and vice-versa
// Triplet form: row 0 holds {rows, columns, number of non-zero elements},
// every other row holds {row index, column index, value} of one non-zero element.
#include <iostream>
using namespace std;

const int MAXN = 20;

struct Triplet {
    int row, col, val;
};

void matrixToTriplet() {
    int r, c, m[MAXN][MAXN];
    do {
        cout << "Rows and columns (max " << MAXN << " each): ";
        cin >> r >> c;
    } while (r < 1 || c < 1 || r > MAXN || c > MAXN);
    cout << "Enter the matrix (" << r << " x " << c << "):" << endl;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++) cin >> m[i][j];

    Triplet t[MAXN * MAXN + 1];
    int k = 1;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            if (m[i][j] != 0) {
                t[k].row = i; t[k].col = j; t[k].val = m[i][j];
                k++;
            }
    t[0].row = r; t[0].col = c; t[0].val = k - 1;

    cout << "\nRow  Col  Value" << endl;
    for (int i = 0; i < k; i++)
        cout << t[i].row << "    " << t[i].col << "    " << t[i].val << endl;
}

void tripletToMatrix() {
    int r, c, k, m[MAXN][MAXN];
    do {
        cout << "Rows and columns of the original matrix (max " << MAXN << " each): ";
        cin >> r >> c;
    } while (r < 1 || c < 1 || r > MAXN || c > MAXN);
    do {
        cout << "Number of non-zero elements: ";
        cin >> k;
    } while (k < 0 || k > r * c);

    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++) m[i][j] = 0;

    cout << "Enter each non-zero element as: row column value (indexes start from 0)" << endl;
    for (int i = 0; i < k; i++) {
        int x, y, val;
        cin >> x >> y >> val;
        if (x < 0 || x >= r || y < 0 || y >= c) {
            cout << "Position out of range, enter again" << endl;
            i--;
            continue;
        }
        m[x][y] = val;
    }

    cout << "\nMatrix:" << endl;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) cout << m[i][j] << "\t";
        cout << endl;
    }
}

int main() {
    int choice;
    do {
        cout << "\n----- MENU -----\n1. Matrix to non-zero (triplet) form\n"
             << "2. Non-zero (triplet) form to matrix\n3. Quit\nChoice: ";
        cin >> choice;
        if (choice == 1) matrixToTriplet();
        else if (choice == 2) tripletToMatrix();
        else if (choice != 3) cout << "Invalid choice" << endl;
    } while (choice != 3);
    return 0;
}
