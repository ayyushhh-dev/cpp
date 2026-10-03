// Q17: Reverse the order of elements in a stack using an additional Queue
// Pop everything into a queue, then dequeue everything back into the same stack.
#include <iostream>
using namespace std;

const int MAX = 100;

class Stack {
    int a[MAX];
    int top;
public:
    Stack() : top(-1) {}
    bool isEmpty() const { return top == -1; }
    void push(int v) { a[++top] = v; }
    int pop() { return a[top--]; }
    void display() const {
        if (isEmpty()) { cout << "(empty)" << endl; return; }
        for (int i = top; i >= 0; i--) cout << a[i] << " ";
        cout << endl;
    }
};

class Queue {
    int a[MAX];
    int front, rear;                         // elements are a[front .. rear-1]
public:
    Queue() : front(0), rear(0) {}
    bool isEmpty() const { return front == rear; }
    void enqueue(int v) { a[rear++] = v; }
    int dequeue() { return a[front++]; }
};

int main() {
    Stack s;
    Queue q;
    int n, v;
    do {
        cout << "How many elements (1-" << MAX << ")? ";
        cin >> n;
    } while (n < 1 || n > MAX);
    cout << "Enter " << n << " elements (last one entered is the top): ";
    for (int i = 0; i < n; i++) { cin >> v; s.push(v); }

    cout << "\nOriginal stack (top to bottom): ";
    s.display();

    while (!s.isEmpty()) q.enqueue(s.pop());     // top element enters the queue first
    while (!q.isEmpty()) s.push(q.dequeue());    // and goes back to the stack first, so it ends at the bottom

    cout << "Reversed stack (top to bottom): ";
    s.display();
    return 0;
}
