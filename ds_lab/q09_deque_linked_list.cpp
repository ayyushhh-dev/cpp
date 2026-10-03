// Q9: Double-ended Queue (Deque) using Linked List (doubly linked)
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* prev;
    Node* next;
    Node(int d) : data(d), prev(NULL), next(NULL) {}
};

class Deque {
    Node* front;
    Node* rear;
public:
    Deque() : front(NULL), rear(NULL) {}
    ~Deque() {
        while (front) { Node* t = front; front = front->next; delete t; }
    }
    bool isEmpty() const { return front == NULL; }
    void insertFront(int v) {
        Node* n = new Node(v);
        if (isEmpty()) { front = rear = n; return; }
        n->next = front;
        front->prev = n;
        front = n;
    }
    void insertRear(int v) {
        Node* n = new Node(v);
        if (isEmpty()) { front = rear = n; return; }
        n->prev = rear;
        rear->next = n;
        rear = n;
    }
    bool deleteFront(int &v) {
        if (isEmpty()) return false;
        Node* t = front;
        v = t->data;
        front = front->next;
        if (front) front->prev = NULL;
        else rear = NULL;
        delete t;
        return true;
    }
    bool deleteRear(int &v) {
        if (isEmpty()) return false;
        Node* t = rear;
        v = t->data;
        rear = rear->prev;
        if (rear) rear->next = NULL;
        else front = NULL;
        delete t;
        return true;
    }
    bool getFront(int &v) const { if (isEmpty()) return false; v = front->data; return true; }
    bool getRear(int &v) const { if (isEmpty()) return false; v = rear->data; return true; }
    void display() const {
        if (isEmpty()) { cout << "Deque is empty" << endl; return; }
        cout << "Deque (front to rear): ";
        for (Node* p = front; p; p = p->next) cout << p->data << " ";
        cout << endl;
    }
};

int main() {
    Deque d;
    int choice, v;
    do {
        cout << "\n----- MENU -----\n1. Insert at front\n2. Insert at rear\n3. Delete from front\n"
             << "4. Delete from rear\n5. Get front element\n6. Get rear element\n7. Display\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: cout << "Value: "; cin >> v; d.insertFront(v); break;
        case 2: cout << "Value: "; cin >> v; d.insertRear(v); break;
        case 3:
            if (d.deleteFront(v)) cout << "Deleted from front: " << v << endl;
            else cout << "Deque is empty" << endl;
            break;
        case 4:
            if (d.deleteRear(v)) cout << "Deleted from rear: " << v << endl;
            else cout << "Deque is empty" << endl;
            break;
        case 5:
            if (d.getFront(v)) cout << "Front element: " << v << endl;
            else cout << "Deque is empty" << endl;
            break;
        case 6:
            if (d.getRear(v)) cout << "Rear element: " << v << endl;
            else cout << "Deque is empty" << endl;
            break;
        case 7: d.display(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
