// Q13: GCD of two numbers - (i) with recursion (ii) without recursion
#include <iostream>
using namespace std;

int gcdRec(int a, int b) {
    if (b == 0) return a;
    return gcdRec(b, a % b);
}

int gcdIter(int a, int b) {
    while (b != 0) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

int main() {
    int a, b;
    cout << "Enter two numbers: ";
    cin >> a >> b;
    if (a < 0) a = -a;
    if (b < 0) b = -b;

    cout << "GCD (recursion): " << gcdRec(a, b) << endl;
    cout << "GCD (iteration): " << gcdIter(a, b) << endl;
    return 0;
}
