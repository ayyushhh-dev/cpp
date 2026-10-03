// Q5: Circular Linked List - insert, delete, search, reverse
// We keep a pointer to the LAST node (tail); tail->next is the first node.
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int d) : data(d), next(NULL) {}
};

class CList {
    Node* tail;
public:
    CList() : tail(NULL) {}
    ~CList() {
        if (!tail) return;
        Node* cur = tail->next;
        tail->next = NULL;                  // break the circle, then delete normally
        while (cur) { Node* t = cur; cur = cur->next; delete t; }
    }

    void insertFront(int v) {
        Node* n = new Node(v);
        if (!tail) { n->next = n; tail = n; return; }
        n->next = tail->next;
        tail->next = n;
    }
    void insertEnd(int v) {
        insertFront(v);
        tail = tail->next;                  // the new node becomes the last one
    }
    bool insertAfter(int key, int v) {
        if (!tail) return false;
        Node* p = tail;
        do {
            p = p->next;
            if (p->data == key) {
                Node* n = new Node(v);
                n->next = p->next;
                p->next = n;
                if (p == tail) tail = n;
                return true;
            }
        } while (p != tail);
        return false;
    }
    bool remove(int v) {
        if (!tail) return false;
        Node* prev = tail;
        Node* cur = tail->next;
        do {
            if (cur->data == v) {
                if (cur == prev) {          // only one node
                    delete cur;
                    tail = NULL;
                } else {
                    prev->next = cur->next;
                    if (cur == tail) tail = prev;
                    delete cur;
                }
                return true;
            }
            prev = cur;
            cur = cur->next;
        } while (prev != tail);
        return false;
    }
    int search(int v) const {
        if (!tail) return -1;
        Node* p = tail->next;
        int pos = 1;
        do {
            if (p->data == v) return pos;
            p = p->next;
            pos++;
        } while (p != tail->next);
        return -1;
    }
    void reverse() {
        if (!tail || tail->next == tail) return;
        Node* head = tail->next;
        Node* prev = tail;
        Node* cur = head;
        do {
            Node* nxt = cur->next;
            cur->next = prev;
            prev = cur;
            cur = nxt;
        } while (cur != head);
        tail = head;                        // old first node is the new last node
    }
    void display() const {
        if (!tail) { cout << "(empty)" << endl; return; }
        Node* p = tail->next;
        do {
            cout << p->data << " -> ";
            p = p->next;
        } while (p != tail->next);
        cout << "(back to first)" << endl;
    }
};

int main() {
    CList list;
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
