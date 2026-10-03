// Q11: Factorial and factors of a number - (i) using recursion (ii) using iteration
#include <iostream>
using namespace std;

unsigned long long factorialRec(int n) {
    if (n <= 1) return 1;
    return n * factorialRec(n - 1);
}

unsigned long long factorialIter(int n) {
    unsigned long long f = 1;
    for (int i = 2; i <= n; i++) f *= i;
    return f;
}

void factorsRec(int n, int i) {
    if (i > n) return;
    if (n % i == 0) cout << i << " ";
    factorsRec(n, i + 1);
}

void factorsIter(int n) {
    for (int i = 1; i <= n; i++)
        if (n % i == 0) cout << i << " ";
}

int main() {
    int n;
    do {
        cout << "Enter a positive number: ";
        cin >> n;
    } while (n < 1);

    if (n <= 20) {                           // 21! does not fit in unsigned long long
        cout << "Factorial (recursion): " << factorialRec(n) << endl;
        cout << "Factorial (iteration): " << factorialIter(n) << endl;
    } else {
        cout << "Factorial skipped: " << n << "! is too large for unsigned long long (max n = 20)" << endl;
    }

    cout << "Factors (recursion): ";
    factorsRec(n, 1);
    cout << "\nFactors (iteration): ";
    factorsIter(n);
    cout << endl;
    return 0;
}
