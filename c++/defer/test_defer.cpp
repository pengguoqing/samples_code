// defer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "defer.h"
using namespace std;

void TestDefer()
{
    defer(cout<<"world!"<<endl);
    cout<<"Hello ";
}

void TestDefer2()
{
    {
        defer(TestDefer());
        cout<<"Test defer in scope"<<endl;
    }
    cout << "Test defer are ok?" << endl;
    defer(cout<<"TestDefer2"<<endl);
}

void TestDefer3(int a)
{
    {
        defer(cout << "test param func " << a << endl);
    }

    defer(cout << "TestDefer3" << endl);
}
int main()
{
    TestDefer2();
    TestDefer3(100);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
