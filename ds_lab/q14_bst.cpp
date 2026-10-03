// Q14: Binary Search Tree with many operations (two trees, so they can be compared)
#include <iostream>
#include <stack>
#include <queue>
using namespace std;

struct Node {
    int data;
    Node* left;
    Node* right;
    Node(int d) : data(d), left(NULL), right(NULL) {}
};

// ---------- insertion ----------
Node* insertRec(Node* r, int v) {
    if (!r) return new Node(v);
    if (v < r->data) r->left = insertRec(r->left, v);
    else if (v > r->data) r->right = insertRec(r->right, v);
    else cout << "Duplicate ignored" << endl;
    return r;
}

void insertIter(Node*& root, int v) {
    Node* n = new Node(v);
    if (!root) { root = n; return; }
    Node *cur = root, *parent = NULL;
    while (cur) {
        parent = cur;
        if (v < cur->data) cur = cur->left;
        else if (v > cur->data) cur = cur->right;
        else { cout << "Duplicate ignored" << endl; delete n; return; }
    }
    if (v < parent->data) parent->left = n;
    else parent->right = n;
}

// ---------- search ----------
bool searchBST(Node* r, int v) {
    while (r) {
        if (v == r->data) return true;
        r = (v < r->data) ? r->left : r->right;
    }
    return false;
}

// ---------- deletion by copying ----------
// Node with two children: copy the inorder predecessor (max of left subtree) into it,
// then delete that predecessor node.
Node* deleteByCopying(Node* r, int key) {
    if (!r) return NULL;
    if (key < r->data) r->left = deleteByCopying(r->left, key);
    else if (key > r->data) r->right = deleteByCopying(r->right, key);
    else {
        if (!r->left) { Node* t = r->right; delete r; return t; }
        if (!r->right) { Node* t = r->left; delete r; return t; }
        Node* pred = r->left;
        while (pred->right) pred = pred->right;
        r->data = pred->data;
        r->left = deleteByCopying(r->left, pred->data);
    }
    return r;
}

// ---------- deletion by merging ----------
// Node with two children: attach the right subtree to the rightmost node of the
// left subtree, then the left subtree takes the place of the deleted node.
Node* deleteByMerging(Node* r, int key) {
    if (!r) return NULL;
    if (key < r->data) r->left = deleteByMerging(r->left, key);
    else if (key > r->data) r->right = deleteByMerging(r->right, key);
    else {
        Node* t = r;
        if (!r->left) r = r->right;
        else if (!r->right) r = r->left;
        else {
            Node* p = r->left;
            while (p->right) p = p->right;
            p->right = r->right;
            r = r->left;
        }
        delete t;
    }
    return r;
}

// ---------- recursive traversals ----------
void preRec(Node* r)  { if (!r) return; cout << r->data << " "; preRec(r->left); preRec(r->right); }
void inRec(Node* r)   { if (!r) return; inRec(r->left); cout << r->data << " "; inRec(r->right); }
void postRec(Node* r) { if (!r) return; postRec(r->left); postRec(r->right); cout << r->data << " "; }

// ---------- iterative traversals ----------
void preIter(Node* r) {
    if (!r) return;
    stack<Node*> s;
    s.push(r);
    while (!s.empty()) {
        Node* t = s.top(); s.pop();
        cout << t->data << " ";
        if (t->right) s.push(t->right);
        if (t->left) s.push(t->left);
    }
}

void inIter(Node* r) {
    stack<Node*> s;
    Node* cur = r;
    while (cur || !s.empty()) {
        while (cur) { s.push(cur); cur = cur->left; }
        cur = s.top(); s.pop();
        cout << cur->data << " ";
        cur = cur->right;
    }
}

void postIter(Node* r) {                     // two stacks
    if (!r) return;
    stack<Node*> s1, s2;
    s1.push(r);
    while (!s1.empty()) {
        Node* t = s1.top(); s1.pop();
        s2.push(t);
        if (t->left) s1.push(t->left);
        if (t->right) s1.push(t->right);
    }
    while (!s2.empty()) { cout << s2.top()->data << " "; s2.pop(); }
}

// ---------- level by level ----------
void levelOrder(Node* r) {
    if (!r) { cout << "Tree is empty" << endl; return; }
    queue<Node*> q;
    q.push(r);
    int level = 0;
    while (!q.empty()) {
        int sz = (int)q.size();
        cout << "Level " << level++ << ": ";
        while (sz--) {
            Node* t = q.front(); q.pop();
            cout << t->data << " ";
            if (t->left) q.push(t->left);
            if (t->right) q.push(t->right);
        }
        cout << endl;
    }
}

// ---------- counts and height ----------
int countLeaf(Node* r) {
    if (!r) return 0;
    if (!r->left && !r->right) return 1;
    return countLeaf(r->left) + countLeaf(r->right);
}
int countNonLeaf(Node* r) {
    if (!r || (!r->left && !r->right)) return 0;
    return 1 + countNonLeaf(r->left) + countNonLeaf(r->right);
}
int height(Node* r) {                        // number of levels; empty tree = 0
    if (!r) return 0;
    int l = height(r->left), rt = height(r->right);
    return 1 + (l > rt ? l : rt);
}

// ---------- mirror image and equality ----------
Node* mirror(Node* r) {                      // builds a NEW tree, original is untouched
    if (!r) return NULL;
    Node* n = new Node(r->data);
    n->left = mirror(r->right);
    n->right = mirror(r->left);
    return n;
}
bool equalTrees(Node* a, Node* b) {
    if (!a && !b) return true;
    if (!a || !b) return false;
    return a->data == b->data && equalTrees(a->left, b->left) && equalTrees(a->right, b->right);
}

void destroy(Node* r) {
    if (!r) return;
    destroy(r->left);
    destroy(r->right);
    delete r;
}

int pickTree() {
    int w;
    do { cout << "Which tree (1 or 2)? "; cin >> w; } while (w != 1 && w != 2);
    return w - 1;
}

int main() {
    Node* root[2] = {NULL, NULL};
    int choice, w, v;
    do {
        cout << "\n----- BST MENU -----\n"
             << " 1. Insert (recursive)\n 2. Insert (iterative)\n 3. Delete by copying\n"
             << " 4. Delete by merging\n 5. Search a number\n 6. Traversals (recursive)\n"
             << " 7. Traversals (iterative)\n 8. Level-by-level traversal\n"
             << " 9. Count leaf and non-leaf nodes\n10. Height of tree\n"
             << "11. Mirror image of tree\n12. Check if tree 1 and tree 2 are equal\n 0. Quit\nChoice: ";
        cin >> choice;
        switch (choice) {
        case 1: w = pickTree(); cout << "Value: "; cin >> v; root[w] = insertRec(root[w], v); break;
        case 2: w = pickTree(); cout << "Value: "; cin >> v; insertIter(root[w], v); break;
        case 3: w = pickTree(); cout << "Value to delete: "; cin >> v;
            if (!searchBST(root[w], v)) cout << v << " not found" << endl;
            else root[w] = deleteByCopying(root[w], v);
            break;
        case 4: w = pickTree(); cout << "Value to delete: "; cin >> v;
            if (!searchBST(root[w], v)) cout << v << " not found" << endl;
            else root[w] = deleteByMerging(root[w], v);
            break;
        case 5: w = pickTree(); cout << "Value to search: "; cin >> v;
            cout << v << (searchBST(root[w], v) ? " found" : " not found") << endl;
            break;
        case 6: w = pickTree();
            cout << "Preorder : "; preRec(root[w]);
            cout << "\nInorder  : "; inRec(root[w]);
            cout << "\nPostorder: "; postRec(root[w]);
            cout << endl;
            break;
        case 7: w = pickTree();
            cout << "Preorder : "; preIter(root[w]);
            cout << "\nInorder  : "; inIter(root[w]);
            cout << "\nPostorder: "; postIter(root[w]);
            cout << endl;
            break;
        case 8: w = pickTree(); levelOrder(root[w]); break;
        case 9: w = pickTree();
            cout << "Leaf nodes: " << countLeaf(root[w])
                 << ", Non-leaf nodes: " << countNonLeaf(root[w]) << endl;
            break;
        case 10: w = pickTree();
            cout << "Height of tree (number of levels): " << height(root[w]) << endl;
            break;
        case 11: {
            w = pickTree();
            Node* m = mirror(root[w]);
            cout << "Mirror image, level by level:" << endl;
            levelOrder(m);
            destroy(m);
            break;
        }
        case 12:
            cout << (equalTrees(root[0], root[1]) ? "The two trees are equal" : "The two trees are NOT equal") << endl;
            break;
        case 0: cout << "Bye!" << endl; break;
        default: cout << "Invalid choice" << endl;
        }
    } while (choice != 0);

    destroy(root[0]);
    destroy(root[1]);
    return 0;
}
