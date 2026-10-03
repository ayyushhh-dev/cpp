// Q10: Menu driven operations on an array
#include <iostream>
using namespace std;

const int MAX = 100;

void readArray(int a[], int &n) {
    do {
        cout << "How many elements (1-" << MAX << ")? ";
        cin >> n;
    } while (n < 1 || n > MAX);
    cout << "Enter " << n << " elements: ";
    for (int i = 0; i < n; i++) cin >> a[i];
}

int main() {
    int a[MAX], n = 0, choice;
    readArray(a, n);

    do {
        cout << "\n----- MENU -----\n"
             << "1. Print even elements\n"
             << "2. Print odd elements\n"
             << "3. Sum and average\n"
             << "4. Maximum and minimum\n"
             << "5. Remove duplicates\n"
             << "6. Print in reverse order\n"
             << "7. Re-enter the array\n"
             << "8. Quit\n"
             << "Choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Even elements: ";
            for (int i = 0; i < n; i++) if (a[i] % 2 == 0) cout << a[i] << " ";
            cout << endl;
            break;
        case 2:
            cout << "Odd elements: ";
            for (int i = 0; i < n; i++) if (a[i] % 2 != 0) cout << a[i] << " ";
            cout << endl;
            break;
        case 3: {
            long long sum = 0;
            for (int i = 0; i < n; i++) sum += a[i];
            cout << "Sum = " << sum << ", Average = " << (double)sum / n << endl;
            break;
        }
        case 4: {
            int mx = a[0], mn = a[0];
            for (int i = 1; i < n; i++) {
                if (a[i] > mx) mx = a[i];
                if (a[i] < mn) mn = a[i];
            }
            cout << "Maximum = " << mx << ", Minimum = " << mn << endl;
            break;
        }
        case 5: {
            int m = 0;                       // size of the unique part
            for (int i = 0; i < n; i++) {
                bool dup = false;
                for (int j = 0; j < m; j++)
                    if (a[j] == a[i]) { dup = true; break; }
                if (!dup) a[m++] = a[i];
            }
            n = m;
            cout << "Array after removing duplicates: ";
            for (int i = 0; i < n; i++) cout << a[i] << " ";
            cout << endl;
            break;
        }
        case 6:
            cout << "Reverse order: ";
            for (int i = n - 1; i >= 0; i--) cout << a[i] << " ";
            cout << endl;
            break;
        case 7:
            readArray(a, n);
            break;
        case 8:
            cout << "Bye!" << endl;
            break;
        default:
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 8);

    return 0;
}
