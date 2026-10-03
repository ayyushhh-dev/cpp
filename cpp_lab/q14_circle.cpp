// Q14: Area and circumference of a circle computed in another function
#include <iostream>
using namespace std;

const double PI = 3.14159265358979;

// area and circumference are passed by reference so main() gets the results
void circle(double r, double &area, double &circumference) {
    area = PI * r * r;
    circumference = 2 * PI * r;
}

int main() {
    double r, area, circumference;
    cout << "Enter radius: ";
    cin >> r;

    circle(r, area, circumference);

    cout << "Area = " << area << endl;
    cout << "Circumference = " << circumference << endl;
    return 0;
}
