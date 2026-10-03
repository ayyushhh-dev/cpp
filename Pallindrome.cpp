#include <iostream>
#include <string>
#include <cctype>
using namespace std;

int main() {
    string s, t;
    cout << "Enter a string: ";
    getline(cin, s);

    for (char c : s)
        if (isalnum((unsigned char)c))
            t += tolower((unsigned char)c);

    int i = 0, j = (int)t.size() - 1;
    bool ok = true;
    while (i < j) {
        if (t[i++] != t[j--]) { ok = false; break; }
    }

    cout << (ok ? "Pallindrome" : "Not a pallindrome") << endl;
    return 0;
}