#include <iostream>
#include <string>
#include <cctype>
using namespace std;

bool startsAndEndsWithS(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    if (first == string::npos) return false;   // empty or only spaces

    return tolower((unsigned char)str[first]) == 's' &&
           tolower((unsigned char)str[last]) == 's';
}

int main() {
    string input;
    cout << "Enter a string: ";
    getline(cin, input);

    if (startsAndEndsWithS(input))
        cout << "YES: starts and ends with s" << endl;
    else
        cout << "NO: does not start and end with s" << endl;

    return 0;
}
