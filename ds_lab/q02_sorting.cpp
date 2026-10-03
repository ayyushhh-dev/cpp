// Q2: Sort a list - Insertion, Bubble or Selection sort
#include <iostream>
using namespace std;

const int MAX = 100;

void insertionSort(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

void bubbleSort(int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j + 1]) {
                int t = a[j]; a[j] = a[j + 1]; a[j + 1] = t;
                swapped = true;
            }
        if (!swapped) break;
    }
}

void selectionSort(int a[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int m = i;
        for (int j = i + 1; j < n; j++)
            if (a[j] < a[m]) m = j;
        int t = a[i]; a[i] = a[m]; a[m] = t;
    }
}

void readArray(int a[], int &n) {
    do {
        cout << "How many elements (1-" << MAX << ")? ";
        cin >> n;
    } while (n < 1 || n > MAX);
    cout << "Enter " << n << " elements: ";
    for (int i = 0; i < n; i++) cin >> a[i];
}

int main() {
    int a[MAX], n;
    readArray(a, n);

    int choice;
    do {
        cout << "\n----- MENU -----\n1. Insertion sort\n2. Bubble sort\n3. Selection sort\n"
             << "4. Enter a new list\n5. Quit\nChoice: ";
        cin >> choice;
        if (choice >= 1 && choice <= 3) {
            int b[MAX];                              // sort a copy so every choice starts from the original
            for (int i = 0; i < n; i++) b[i] = a[i];
            if (choice == 1) insertionSort(b, n);
            else if (choice == 2) bubbleSort(b, n);
            else selectionSort(b, n);
            cout << "Sorted list: ";
            for (int i = 0; i < n; i++) cout << b[i] << " ";
            cout << endl;
        } else if (choice == 4) readArray(a, n);
        else if (choice != 5) cout << "Invalid choice" << endl;
    } while (choice != 5);
    return 0;
}
