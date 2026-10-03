// Q6: Factors of a given number
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Enter a positive number: ";
    cin >> n;

    cout << "Factors of " << n << ": ";
    for (int i = 1; i <= n; i++)
        if (n % i == 0) cout << i << " ";
    cout << endl;
    return 0;
}
