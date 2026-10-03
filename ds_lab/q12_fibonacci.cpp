// Q12: Fibonacci series - (i) using recursion (ii) using iteration
#include <iostream>
using namespace std;

int fibRec(int n) {                          // n-th term, starting 0, 1, 1, 2, 3, ...
    if (n <= 1) return n;
    return fibRec(n - 1) + fibRec(n - 2);
}

void fibIter(int terms) {
    long long a = 0, b = 1;
    for (int i = 0; i < terms; i++) {
        cout << a << " ";
        long long next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    int n;
    do {
        cout << "How many terms? ";
        cin >> n;
    } while (n < 1);

    cout << "Using recursion: ";
    for (int i = 0; i < n; i++) cout << fibRec(i) << " ";
    cout << "\nUsing iteration: ";
    fibIter(n);
    cout << endl;
    return 0;
}
