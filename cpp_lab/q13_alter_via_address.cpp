// Q13: A function receives the addresses of two variables and alters their contents
#include <iostream>
using namespace std;

void alter(int *x, int *y) {
    *x = *x + 10;       // add 10 to first variable
    *y = *y * 2;        // double the second variable
}

int main() {
    int a = 5, b = 7;
    cout << "Before: a = " << a << ", b = " << b << endl;
    alter(&a, &b);
    cout << "After:  a = " << a << ", b = " << b << endl;
    return 0;
}
