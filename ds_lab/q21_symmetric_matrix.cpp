// Q21: Symmetric Matrix using a one-dimensional array
// Since A[i][j] == A[j][i], only the lower triangle (i >= j) is stored row by row.
// Position of (i, j) in the array (0-based): max*(max+1)/2 + min, where max = larger
// and min = smaller of i and j. Size = n*(n+1)/2.
#include <iostream>
using namespace std;

class SymmetricMatrix {
    int n;
    int* a;
    int index(int i, int j) const {
        int mx = (i > j) ? i : j;
        int mn = (i > j) ? j : i;
        return mx * (mx + 1) / 2 + mn;
    }
public:
    SymmetricMatrix(int size) : n(size) {
        int total = n * (n + 1) / 2;
        a = new int[total];
        for (int k = 0; k < total; k++) a[k] = 0;
    }
    ~SymmetricMatrix() { delete[] a; }

    int get(int i, int j) const { return a[index(i, j)]; }
    void set(int i, int j, int v) { a[index(i, j)] = v; }   // (i,j) and (j,i) change together
    void display() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cout << get(i, j) << "\t";
            cout << endl;
        }
    }
    void showArray() const {
        int total = n * (n + 1) / 2;
        cout << "1D array (" << total << " elements): ";
        for (int k = 0; k < total; k++) cout << a[k] << " ";
        cout << endl;
    }
};

int main() {
    int n;
    do {
        cout << "Order of the matrix (n x n): ";
        cin >> n;
    } while (n < 1);

    SymmetricMatrix m(n);
    cout << "Enter the lower triangular part row by row (" << n * (n + 1) / 2 << " elements):" << endl;
    for (int i = 0; i < n; i++)
        for (int j = 0; j <= i; j++) { int v; cin >> v; m.set(i, j, v); }

    int choice, i, j, v;
    do {
        cout << "\n----- MENU -----\n1. Display matrix\n2. Get an element\n3. Set an element\n"
             << "4. Show the 1D array\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: m.display(); break;
        case 2:
            cout << "Row and column (1 to " << n << "): "; cin >> i >> j;
            if (i < 1 || j < 1 || i > n || j > n) cout << "Out of range" << endl;
            else cout << "Element = " << m.get(i - 1, j - 1) << endl;
            break;
        case 3:
            cout << "Row, column and new value: "; cin >> i >> j >> v;
            if (i < 1 || j < 1 || i > n || j > n) cout << "Out of range" << endl;
            else { m.set(i - 1, j - 1, v); cout << "Set (also updates the mirrored position)" << endl; }
            break;
        case 4: m.showArray(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
