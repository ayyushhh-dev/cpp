// Q1: Search an element in a list - Linear or Binary search
#include <iostream>
using namespace std;

const int MAX = 100;

int linearSearch(int a[], int n, int key) {
    for (int i = 0; i < n; i++)
        if (a[i] == key) return i;
    return -1;
}

// Binary search works only on a SORTED (ascending) array
int binarySearch(int a[], int n, int key) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (a[mid] == key) return mid;
        if (a[mid] < key) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void sortAscending(int a[], int n) {        // bubble sort
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j + 1]) { int t = a[j]; a[j] = a[j + 1]; a[j + 1] = t; }
}

int main() {
    int a[MAX], n;
    do {
        cout << "How many elements (1-" << MAX << ")? ";
        cin >> n;
    } while (n < 1 || n > MAX);
    cout << "Enter " << n << " elements: ";
    for (int i = 0; i < n; i++) cin >> a[i];

    int choice;
    do {
        cout << "\n----- MENU -----\n1. Linear search\n2. Binary search\n3. Quit\nChoice: ";
        cin >> choice;
        if (choice == 1 || choice == 2) {
            int key;
            cout << "Element to search: ";
            cin >> key;
            if (choice == 1) {
                int pos = linearSearch(a, n, key);
                if (pos == -1) cout << key << " not found" << endl;
                else cout << key << " found at position " << pos + 1 << endl;
            } else {
                int b[MAX];                          // sort a copy, keep the original
                for (int i = 0; i < n; i++) b[i] = a[i];
                sortAscending(b, n);
                cout << "Sorted list: ";
                for (int i = 0; i < n; i++) cout << b[i] << " ";
                cout << endl;
                int pos = binarySearch(b, n, key);
                if (pos == -1) cout << key << " not found" << endl;
                else cout << key << " found at position " << pos + 1 << " of the sorted list" << endl;
            }
        } else if (choice != 3) cout << "Invalid choice" << endl;
    } while (choice != 3);
    return 0;
}
