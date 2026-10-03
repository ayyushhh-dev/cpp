// Q1: Sum and product of digits of an integer
#include <iostream>
using namespace std;

int main() {
    long long n;
    cout << "Enter an integer: ";
    cin >> n;

    if (n < 0) n = -n;              // ignore the sign
    long long sum = 0, product = 1;
    if (n == 0) product = 0;

    while (n > 0) {
        int digit = n % 10;         // last digit
        sum += digit;
        product *= digit;
        n /= 10;                    // drop last digit
    }

    cout << "Sum of digits: " << sum << endl;
    cout << "Product of digits: " << product << endl;
    return 0;
}
