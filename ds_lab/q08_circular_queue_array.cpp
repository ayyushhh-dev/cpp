// Q8: Queue operations using Circular Array implementation
#include <iostream>
using namespace std;

const int SIZE = 5;                         // small size so wrap-around is easy to see

class CircularQueue {
    int a[SIZE];
    int front, rear, count;
public:
    CircularQueue() : front(0), rear(SIZE - 1), count(0) {}
    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == SIZE; }
    bool enqueue(int v) {
        if (isFull()) return false;
        rear = (rear + 1) % SIZE;           // wrap around
        a[rear] = v;
        count++;
        return true;
    }
    bool dequeue(int &v) {
        if (isEmpty()) return false;
        v = a[front];
        front = (front + 1) % SIZE;
        count--;
        return true;
    }
    bool peek(int &v) const {
        if (isEmpty()) return false;
        v = a[front];
        return true;
    }
    void display() const {
        if (isEmpty()) { cout << "Queue is empty" << endl; return; }
        cout << "Queue (front to rear): ";
        for (int i = 0; i < count; i++) cout << a[(front + i) % SIZE] << " ";
        cout << endl;
    }
};

int main() {
    CircularQueue q;
    int choice, v;
    do {
        cout << "\n----- MENU -----\n1. Enqueue\n2. Dequeue\n3. Front element\n4. Check empty / full\n"
             << "5. Display\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "Value: "; cin >> v;
            if (!q.enqueue(v)) cout << "Queue overflow (full)" << endl;
            break;
        case 2:
            if (q.dequeue(v)) cout << "Dequeued: " << v << endl;
            else cout << "Queue underflow (empty)" << endl;
            break;
        case 3:
            if (q.peek(v)) cout << "Front element: " << v << endl;
            else cout << "Queue is empty" << endl;
            break;
        case 4:
            cout << "Empty: " << (q.isEmpty() ? "yes" : "no")
                 << ", Full: " << (q.isFull() ? "yes" : "no") << endl;
            break;
        case 5: q.display(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
