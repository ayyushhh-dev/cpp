// Q5: Function to check whether a string is a palindrome
#include <iostream>
#include <string>
using namespace std;

bool isPalindrome(const string& s) {
    int i = 0, j = (int)s.length() - 1;
    while (i < j) {
        if (s[i] != s[j]) return false;
        i++;
        j--;
    }
    return true;
}

int main() {
    string s;
    cout << "Enter a string: ";
    getline(cin, s);
    if (isPalindrome(s)) cout << "\"" << s << "\" is a palindrome" << endl;
    else                 cout << "\"" << s << "\" is not a palindrome" << endl;
    return 0;
}
