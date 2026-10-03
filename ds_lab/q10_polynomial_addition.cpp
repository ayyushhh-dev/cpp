// Q10: Scan polynomials using linked lists and add them
// Each node stores one term (coefficient, exponent); terms are kept in decreasing exponent order.
#include <iostream>
using namespace std;

struct Term {
    int coef, exp;
    Term* next;
    Term(int c, int e) : coef(c), exp(e), next(NULL) {}
};

class Polynomial {
    Term* head;
public:
    Polynomial() : head(NULL) {}
    ~Polynomial() {
        while (head) { Term* t = head; head = head->next; delete t; }
    }

    // inserts a term in order; terms with the same exponent are combined
    void addTerm(int c, int e) {
        if (c == 0) return;
        Term *prev = NULL, *cur = head;
        while (cur && cur->exp > e) { prev = cur; cur = cur->next; }
        if (cur && cur->exp == e) {
            cur->coef += c;
            if (cur->coef == 0) {           // term cancelled out
                if (prev) prev->next = cur->next;
                else head = cur->next;
                delete cur;
            }
            return;
        }
        Term* t = new Term(c, e);
        t->next = cur;
        if (prev) prev->next = t;
        else head = t;
    }

    void read() {
        int n, c, e;
        cout << "Number of terms: ";
        cin >> n;
        for (int i = 1; i <= n; i++) {
            cout << "Term " << i << " (coefficient exponent): ";
            cin >> c >> e;
            addTerm(c, e);
        }
    }

    // this = a + b
    void setSum(const Polynomial& a, const Polynomial& b) {
        for (Term* p = a.head; p; p = p->next) addTerm(p->coef, p->exp);
        for (Term* p = b.head; p; p = p->next) addTerm(p->coef, p->exp);
    }

    void display() const {
        if (!head) { cout << "0" << endl; return; }
        for (Term* t = head; t; t = t->next) {
            int c = t->coef;
            if (t == head) { if (c < 0) cout << "-"; }
            else cout << (c < 0 ? " - " : " + ");
            int ac = c < 0 ? -c : c;
            if (ac != 1 || t->exp == 0) cout << ac;
            if (t->exp > 0) {
                cout << "x";
                if (t->exp > 1) cout << "^" << t->exp;
            }
        }
        cout << endl;
    }
};

int main() {
    Polynomial p1, p2, sum;
    cout << "First polynomial:" << endl;
    p1.read();
    cout << "Second polynomial:" << endl;
    p2.read();

    cout << "\nP1 = "; p1.display();
    cout << "P2 = "; p2.display();
    sum.setSum(p1, p2);
    cout << "P1 + P2 = "; sum.display();
    return 0;
}
