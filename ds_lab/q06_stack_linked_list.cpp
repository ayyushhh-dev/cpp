// Q6: Stack operations using Linked List implementation
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int d) : data(d), next(NULL) {}
};

class Stack {
    Node* top;
public:
    Stack() : top(NULL) {}
    ~Stack() {
        while (top) { Node* t = top; top = top->next; delete t; }
    }
    bool isEmpty() const { return top == NULL; }
    void push(int v) {
        Node* n = new Node(v);
        n->next = top;
        top = n;
    }
    bool pop(int &v) {
        if (isEmpty()) return false;
        Node* t = top;
        v = t->data;
        top = top->next;
        delete t;
        return true;
    }
    bool peek(int &v) const {
        if (isEmpty()) return false;
        v = top->data;
        return true;
    }
    void display() const {
        if (isEmpty()) { cout << "Stack is empty" << endl; return; }
        cout << "Stack (top to bottom): ";
        for (Node* p = top; p; p = p->next) cout << p->data << " ";
        cout << endl;
    }
};

int main() {
    Stack s;
    int choice, v;
    do {
        cout << "\n----- MENU -----\n1. Push\n2. Pop\n3. Peek (top element)\n4. Check if empty\n"
             << "5. Display\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: cout << "Value: "; cin >> v; s.push(v); break;
        case 2:
            if (s.pop(v)) cout << "Popped: " << v << endl;
            else cout << "Stack underflow (empty)" << endl;
            break;
        case 3:
            if (s.peek(v)) cout << "Top element: " << v << endl;
            else cout << "Stack is empty" << endl;
            break;
        case 4: cout << (s.isEmpty() ? "Stack is empty" : "Stack is not empty") << endl; break;
        case 5: s.display(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
