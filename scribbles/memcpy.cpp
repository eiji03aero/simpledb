#include <iostream>
#include <cstring>

using namespace std;

int main()
{
    void* page[4];
    int a {300};
    int b;
    int c;

    memcpy(&(page[0]), &a, 2);
    memcpy(&b, &(page[0]), 2);
    // prints 300
    cout << b << endl;

    memcpy(&c, &(page[0]), 1);
    // prints 44
    cout << c << endl;

    return 0;
}
