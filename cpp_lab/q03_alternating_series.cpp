// Q3: Sum of first n terms of S = 1 - 2 + 3 - 4 + 5 ...
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Enter number of terms: ";
    cin >> n;

    long long sum = 0;
    for (int i = 1; i <= n; i++) {
        if (i % 2 == 1) sum += i;   // odd term: plus
        else            sum -= i;   // even term: minus
    }
    cout << "Sum of first " << n << " terms = " << sum << endl;
    return 0;
}
