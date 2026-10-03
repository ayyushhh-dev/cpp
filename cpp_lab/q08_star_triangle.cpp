#include <iostream>
using namespace std;

int main() {
    int lines;
    cout << "Enter number of lines: ";
    cin >> lines;

    for (int i = 1; i <= lines; i++) {
        for (int s = 1; s <= lines - i; s++) cout << " ";       // leading spaces
        for (int k = 1; k <= 2 * i - 1; k++) cout << "*";       // stars
        cout << endl;
    }
    return 0;
}
