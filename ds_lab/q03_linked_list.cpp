// Q3: Singly Linked List - insert, delete, search, reverse, concatenate (function + operator +)
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int d) : data(d), next(NULL) {}
};

class LinkedList {
    Node* head;
public:
    LinkedList() : head(NULL) {}
    LinkedList(const LinkedList& o) : head(NULL) { copyFrom(o); }          // copy constructor
    LinkedList& operator=(const LinkedList& o) {
        if (this != &o) { clear(); copyFrom(o); }
        return *this;
    }
    ~LinkedList() { clear(); }

    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
    }
    void copyFrom(const LinkedList& o) {
        for (Node* p = o.head; p; p = p->next) insertEnd(p->data);
    }

    void insertFront(int v) {
        Node* n = new Node(v);
        n->next = head;
        head = n;
    }
    void insertEnd(int v) {
        Node* n = new Node(v);
        if (!head) { head = n; return; }
        Node* p = head;
        while (p->next) p = p->next;
        p->next = n;
    }
    bool insertAfter(int key, int v) {
        Node* p = head;
        while (p && p->data != key) p = p->next;
        if (!p) return false;
        Node* n = new Node(v);
        n->next = p->next;
        p->next = n;
        return true;
    }
    bool remove(int v) {
        Node *cur = head, *prev = NULL;
        while (cur && cur->data != v) { prev = cur; cur = cur->next; }
        if (!cur) return false;
        if (prev) prev->next = cur->next;
        else head = cur->next;
        delete cur;
        return true;
    }
    int search(int v) const {                   // returns position (1-based) or -1
        int pos = 1;
        for (Node* p = head; p; p = p->next, pos++)
            if (p->data == v) return pos;
        return -1;
    }
    void reverse() {
        Node *prev = NULL, *cur = head;
        while (cur) {
            Node* nxt = cur->next;
            cur->next = prev;
            prev = cur;
            cur = nxt;
        }
        head = prev;
    }
    // appends a copy of list o at the end of this list
    void concatenate(const LinkedList& o) {
        LinkedList tmp(o);                      // copy first, so list + itself is safe
        for (Node* p = tmp.head; p; p = p->next) insertEnd(p->data);
    }
    // operator + : returns a NEW list = this list followed by o (originals unchanged)
    LinkedList operator+(const LinkedList& o) const {
        LinkedList result(*this);
        result.concatenate(o);
        return result;
    }
    void display() const {
        if (!head) { cout << "(empty)" << endl; return; }
        for (Node* p = head; p; p = p->next) cout << p->data << (p->next ? " -> " : "");
        cout << endl;
    }
};

int pickList() {
    int w;
    do { cout << "Which list (1 or 2)? "; cin >> w; } while (w != 1 && w != 2);
    return w - 1;
}

int main() {
    LinkedList L[2];
    int choice;
    do {
        cout << "\n----- MENU -----\n"
             << "1. Insert at beginning\n2. Insert at end\n3. Insert after a value\n"
             << "4. Delete a value\n5. Search a value\n6. Reverse\n"
             << "7. Concatenate list 2 to list 1 (function)\n"
             << "8. Show list 1 + list 2 (operator +)\n9. Display both lists\n0. Quit\nChoice: ";
        cin >> choice;
        int w, v, key;
        switch (choice) {
        case 1: w = pickList(); cout << "Value: "; cin >> v; L[w].insertFront(v); break;
        case 2: w = pickList(); cout << "Value: "; cin >> v; L[w].insertEnd(v); break;
        case 3:
            w = pickList();
            cout << "Insert after which value? "; cin >> key;
            cout << "New value: "; cin >> v;
            if (!L[w].insertAfter(key, v)) cout << key << " not found" << endl;
            break;
        case 4:
            w = pickList(); cout << "Value to delete: "; cin >> v;
            if (!L[w].remove(v)) cout << v << " not found" << endl;
            break;
        case 5: {
            w = pickList(); cout << "Value to search: "; cin >> v;
            int pos = L[w].search(v);
            if (pos == -1) cout << v << " not found" << endl;
            else cout << v << " found at position " << pos << endl;
            break;
        }
        case 6: w = pickList(); L[w].reverse(); cout << "Reversed" << endl; break;
        case 7: L[0].concatenate(L[1]); cout << "List 1 is now: "; L[0].display(); break;
        case 8: {
            LinkedList sum = L[0] + L[1];
            cout << "List 1 + List 2 = "; sum.display();
            break;
        }
        case 9:
            cout << "List 1: "; L[0].display();
            cout << "List 2: "; L[1].display();
            break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);
    return 0;
}
