// Q15: Sum of n elements using dynamic memory allocation
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;

    int *arr = new int[n];          // allocate n integers on the heap
    cout << "Enter " << n << " elements: ";
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
        sum += arr[i];
    }
    cout << "Sum = " << sum << endl;

    delete[] arr;                   // free the memory
    return 0;
}
