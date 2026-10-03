// Q4: Doubly Linked List - insert, delete, search, reverse
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* prev;
    Node* next;
    Node(int d) : data(d), prev(NULL), next(NULL) {}
};

class DList {
    Node* head;
    Node* tail;
public:
    DList() : head(NULL), tail(NULL) {}
    ~DList() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }

    void insertFront(int v) {
        Node* n = new Node(v);
        if (!head) { head = tail = n; return; }
        n->next = head;
        head->prev = n;
        head = n;
    }
    void insertEnd(int v) {
        Node* n = new Node(v);
        if (!tail) { head = tail = n; return; }
        n->prev = tail;
        tail->next = n;
        tail = n;
    }
    bool insertAfter(int key, int v) {
        Node* p = head;
        while (p && p->data != key) p = p->next;
        if (!p) return false;
        Node* n = new Node(v);
        n->prev = p;
        n->next = p->next;
        if (p->next) p->next->prev = n;
        else tail = n;
        p->next = n;
        return true;
    }
    bool remove(int v) {
        Node* p = head;
        while (p && p->data != v) p = p->next;
        if (!p) return false;
        if (p->prev) p->prev->next = p->next;
        else head = p->next;
        if (p->next) p->next->prev = p->prev;
        else tail = p->prev;
        delete p;
        return true;
    }
    int search(int v) const {
        int pos = 1;
        for (Node* p = head; p; p = p->next, pos++)
            if (p->data == v) return pos;
        return -1;
    }
    void reverse() {
        // swap prev and next of every node, then swap head and tail
        Node* cur = head;
        while (cur) {
            Node* t = cur->prev;
            cur->prev = cur->next;
            cur->next = t;
            cur = cur->prev;                 // old "next"
        }
        Node* t = head; head = tail; tail = t;
    }
    void display() const {
        if (!head) { cout << "(empty)" << endl; return; }
        cout << "Forward : ";
        for (Node* p = head; p; p = p->next) cout << p->data << " ";
        cout << "\nBackward: ";
        for (Node* p = tail; p; p = p->prev) cout << p->data << " ";
        cout << endl;
    }
};

int main() {
    DList list;
    int choice, v, key;
    do {
        cout << "\n----- MENU -----\n1. Insert at beginning\n2. Insert at end\n3. Insert after a value\n"
             << "4. Delete a value\n5. Search a value\n6. Reverse\n7. Display\n0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: cout << "Value: "; cin >> v; list.insertFront(v); break;
        case 2: cout << "Value: "; cin >> v; list.insertEnd(v); break;
        case 3:
            cout << "Insert after which value? "; cin >> key;
            cout << "New value: "; cin >> v;
            if (!list.insertAfter(key, v)) cout << key << " not found" << endl;
            break;
        case 4:
            cout << "Value to delete: "; cin >> v;
            if (!list.remove(v)) cout << v << " not found" << endl;
            break;
        case 5: {
            cout << "Value to search: "; cin >> v;
            int pos = list.search(v);
            if (pos == -1) cout << v << " not found" << endl;
            else cout << v << " found at position " << pos << endl;
            break;
        }
        case 6: list.reverse(); cout << "Reversed" << endl; break;
        case 7: list.display(); break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
