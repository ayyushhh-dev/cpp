// Q16: Reverse the order of elements in a stack using an additional stack
// Popping every element of the first stack and pushing it onto the additional
// stack reverses the order: the additional stack now holds the reversed stack.
#include <iostream>
using namespace std;

const int MAX = 100;

class Stack {
    int a[MAX];
    int top;
public:
    Stack() : top(-1) {}
    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == MAX - 1; }
    void push(int v) { a[++top] = v; }
    int pop() { return a[top--]; }
    void display() const {
        if (isEmpty()) { cout << "(empty)" << endl; return; }
        for (int i = top; i >= 0; i--) cout << a[i] << " ";
        cout << endl;
    }
};

int main() {
    Stack s, extra;
    int n, v;
    do {
        cout << "How many elements (1-" << MAX << ")? ";
        cin >> n;
    } while (n < 1 || n > MAX);
    cout << "Enter " << n << " elements (last one entered is the top): ";
    for (int i = 0; i < n; i++) { cin >> v; s.push(v); }

    cout << "\nOriginal stack (top to bottom): ";
    s.display();

    while (!s.isEmpty()) extra.push(s.pop());

    cout << "Reversed stack (top to bottom): ";
    extra.display();
    return 0;
}
