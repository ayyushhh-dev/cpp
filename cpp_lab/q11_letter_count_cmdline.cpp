// Q11: Count occurrences of each alphabet in text given as command line arguments
// Compile: g++ q11_letter_count_cmdline.cpp -o q11
// Run:     ./q11 hello world       (Windows: q11 hello world)
#include <iostream>
#include <cctype>
using namespace std;

int main(int argc, char* argv[]) {
    // argc = number of arguments, argv[0] = program name, argv[1..] = the text
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <text> [more text...]" << endl;
        return 1;
    }

    int count[26] = {0};
    for (int i = 1; i < argc; i++) {
        for (char* p = argv[i]; *p != '\0'; p++) {
            if (isalpha((unsigned char)*p))
                count[tolower((unsigned char)*p) - 'a']++;
        }
    }

    cout << "Letter  Count" << endl;
    for (int i = 0; i < 26; i++)
        if (count[i] > 0)
            cout << "  " << (char)('a' + i) << "       " << count[i] << endl;
    return 0;
}
