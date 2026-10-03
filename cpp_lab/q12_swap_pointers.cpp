// Q12: Swap two numbers using pointers
#include <iostream>
using namespace std;

void swapNumbers(int *p, int *q) {
    int temp = *p;      // *p = value stored at address p
    *p = *q;
    *q = temp;
}

int main() {
    int a, b;
    cout << "Enter two numbers: ";
    cin >> a >> b;

    cout << "Before swap: a = " << a << ", b = " << b << endl;
    swapNumbers(&a, &b);    // &a = address of a
    cout << "After swap:  a = " << a << ", b = " << b << endl;
    return 0;
}
