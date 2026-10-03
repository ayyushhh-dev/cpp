// Q16: Menu driven operations on strings
#include <iostream>
#include <cstring>
using namespace std;

// a. print the address of each character
void showAddresses(const char *s) {
    for (const char *p = s; *p != '\0'; p++)
        cout << "'" << *p << "' is at " << (void*)p << endl;
}

// b. concatenate without strcat: go to the end of dest, then copy src
void concatManual(char *dest, const char *src) {
    while (*dest != '\0') dest++;
    while (*src != '\0') { *dest = *src; dest++; src++; }
    *dest = '\0';
}

// e. length using pointers
int lengthPtr(const char *s) {
    const char *p = s;
    while (*p != '\0') p++;
    return (int)(p - s);
}

// f. lowercase to uppercase
void toUpper(char *s) {
    for (; *s != '\0'; s++)
        if (*s >= 'a' && *s <= 'z') *s = *s - 'a' + 'A';
}

int main() {
    char s1[100], s2[100], result[200];
    cout << "Enter first string: ";
    cin.getline(s1, 100);
    cout << "Enter second string: ";
    cin.getline(s2, 100);

    int choice;
    do {
        cout << "\n----- MENU -----\n"
             << "1. Show address of each character (first string)\n"
             << "2. Concatenate without strcat\n"
             << "3. Concatenate using strcat\n"
             << "4. Compare the two strings\n"
             << "5. Length of both strings (pointers)\n"
             << "6. Convert to uppercase\n"
             << "7. Quit\n"
             << "Choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            showAddresses(s1);
            break;
        case 2:
            strcpy(result, s1);
            concatManual(result, s2);
            cout << "Result: " << result << endl;
            break;
        case 3:
            strcpy(result, s1);
            strcat(result, s2);
            cout << "Result: " << result << endl;
            break;
        case 4: {
            int c = strcmp(s1, s2);
            if (c == 0)      cout << "Strings are equal" << endl;
            else if (c < 0)  cout << "First string is smaller" << endl;
            else             cout << "First string is greater" << endl;
            break;
        }
        case 5:
            cout << "Length of first string: " << lengthPtr(s1) << endl;
            cout << "Length of second string: " << lengthPtr(s2) << endl;
            break;
        case 6:
            strcpy(result, s1);
            toUpper(result);
            cout << "First string in uppercase: " << result << endl;
            strcpy(result, s2);
            toUpper(result);
            cout << "Second string in uppercase: " << result << endl;
            break;
        case 7:
            cout << "Bye!" << endl;
            break;
        default:
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 7);

    return 0;
}
