// Q18: Diagonal Matrix using a one-dimensional array
// Only the n diagonal elements are stored: element (i, j) is a[i] if i == j, else 0.
#include <iostream>
using namespace std;

class DiagonalMatrix {
    int n;
    int* a;
public:
    DiagonalMatrix(int size) : n(size) { a = new int[n]; for (int i = 0; i < n; i++) a[i] = 0; }
    ~DiagonalMatrix() { delete[] a; }

    int get(int i, int j) const { return (i == j) ? a[i] : 0; }
    bool set(int i, int j, int v) {          // only diagonal positions can hold a non-zero value
        if (i == j) { a[i] = v; return true; }
        return v == 0;
    }
    void display() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cout << get(i, j) << "\t";
            cout << endl;
        }
    }
    void showArray() const {
        cout << "1D array (" << n << " elements): ";
        for (int i = 0; i < n; i++) cout << a[i] << " ";
        cout << endl;
    }
    int size() const { return n; }
};

int main() {
    int n;
    do {
        cout << "Order of the matrix (n x n): ";
        cin >> n;
    } while (n < 1);

    DiagonalMatrix m(n);
    cout << "Enter the " << n << " diagonal elements: ";
    for (int i = 0; i < n; i++) { int v; cin >> v; m.set(i, i, v); }

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
            else if (!m.set(i - 1, j - 1, v)) cout << "Not allowed: off-diagonal elements must be 0" << endl;
            break;
        case 4: m.showArray(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
