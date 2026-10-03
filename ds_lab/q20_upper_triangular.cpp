// Q20: Upper Triangular Matrix using a one-dimensional array
// Elements on and above the diagonal (j >= i) are stored row by row.
// Position of (i, j) in the array (0-based): i*n - i*(i-1)/2 + (j - i). Size = n*(n+1)/2.
#include <iostream>
using namespace std;

class UpperTriangular {
    int n;
    int* a;
    int index(int i, int j) const { return i * n - i * (i - 1) / 2 + (j - i); }
public:
    UpperTriangular(int size) : n(size) {
        int total = n * (n + 1) / 2;
        a = new int[total];
        for (int k = 0; k < total; k++) a[k] = 0;
    }
    ~UpperTriangular() { delete[] a; }

    int get(int i, int j) const { return (j >= i) ? a[index(i, j)] : 0; }
    bool set(int i, int j, int v) {
        if (j >= i) { a[index(i, j)] = v; return true; }
        return v == 0;                       // lower part is always 0
    }
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

    UpperTriangular m(n);
    cout << "Enter the upper triangular part row by row (" << n * (n + 1) / 2 << " elements):" << endl;
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) { int v; cin >> v; m.set(i, j, v); }

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
            else if (!m.set(i - 1, j - 1, v)) cout << "Not allowed: elements below the diagonal must be 0" << endl;
            break;
        case 4: m.showArray(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
