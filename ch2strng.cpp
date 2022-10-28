#include <bits/stdc++.h>

using namespace std;

int main()
{

    int *ptr;
    int arr[] = {6, 7, 8, 9, 0};

    ptr = arr;

    for (int i = 0; i < 5; i++)
        cout << *ptr++;

    return 0;
}