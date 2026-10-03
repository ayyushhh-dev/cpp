// Q7: Stack operations using Array implementation
#include <iostream>
using namespace std;

const int SIZE = 10;

class Stack {
    int a[SIZE];
    int top;                                // index of top element, -1 when empty
public:
    Stack() : top(-1) {}
    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == SIZE - 1; }
    bool push(int v) {
        if (isFull()) return false;
        a[++top] = v;
        return true;
    }
    bool pop(int &v) {
        if (isEmpty()) return false;
        v = a[top--];
        return true;
    }
    bool peek(int &v) const {
        if (isEmpty()) return false;
        v = a[top];
        return true;
    }
    void display() const {
        if (isEmpty()) { cout << "Stack is empty" << endl; return; }
        cout << "Stack (top to bottom): ";
        for (int i = top; i >= 0; i--) cout << a[i] << " ";
        cout << endl;
    }
};

int main() {
    Stack s;
    int choice, v;
    do {
        cout << "\n----- MENU -----\n1. Push\n2. Pop\n3. Peek (top element)\n4. Check empty / full\n"
             << "5. Display\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "Value: "; cin >> v;
            if (!s.push(v)) cout << "Stack overflow (full)" << endl;
            break;
        case 2:
            if (s.pop(v)) cout << "Popped: " << v << endl;
            else cout << "Stack underflow (empty)" << endl;
            break;
        case 3:
            if (s.peek(v)) cout << "Top element: " << v << endl;
            else cout << "Stack is empty" << endl;
            break;
        case 4:
            cout << "Empty: " << (s.isEmpty() ? "yes" : "no")
                 << ", Full: " << (s.isFull() ? "yes" : "no") << endl;
            break;
        case 5: s.display(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
