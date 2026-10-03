// Q2: Reverse a non-negative integer
#include <iostream>
using namespace std;

int main() {
    long long n;
    do {
        cout << "Enter a non-negative integer: ";
        cin >> n;
    } while (n < 0);

    long long rev = 0;
    while (n > 0) {
        rev = rev * 10 + n % 10;
        n /= 10;
    }
    cout << "Reversed number: " << rev << endl;
    return 0;
}
