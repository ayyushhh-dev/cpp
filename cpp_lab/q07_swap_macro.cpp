// Q7: Swap two numbers using a macro
#include <iostream>
using namespace std;

// Macro: t = data type, a and b = the two variables.
// do { } while(0) makes the macro behave like one normal statement.
#define SWAP(t, a, b) do { t temp = a; a = b; b = temp; } while (0)

int main() {
    int x, y;
    cout << "Enter two numbers: ";
    cin >> x >> y;

    cout << "Before swap: x = " << x << ", y = " << y << endl;
    SWAP(int, x, y);
    cout << "After swap:  x = " << x << ", y = " << y << endl;
    return 0;
}
