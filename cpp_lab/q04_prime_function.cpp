// Q4: Function to check prime; use it to print primes below 100
#include <iostream>
using namespace std;

bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int main() {
    int n;
    cout << "Enter a number to check: ";
    cin >> n;
    cout << n << (isPrime(n) ? " is prime" : " is not prime") << endl;

    cout << "Primes less than 100:" << endl;
    for (int i = 2; i < 100; i++)
        if (isPrime(i)) cout << i << " ";
    cout << endl;
    return 0;
}
