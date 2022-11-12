// Event.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "event.h"

using namespace std;

static CXEvent kEvent;
static constexpr int testCnt = 3;

void TestEventFunc1()
{
    for (int i=0; i< testCnt; i++)
    {
        
		if (i == testCnt-1)
		{
			auto begin = chrono::high_resolution_clock::now();
			kEvent.tryWait(5000ms);
			auto end = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(end - begin);
			cout << "TestEventFunc1"<<"tryWait time:"<< duration.count() << endl;
		}
		else
		{
			kEvent.Wait();
			cout << "TestEventFunc1 Wait" << endl;
		}
       
    }

}
void TestEventFunc2()
{
	for (int i=0; i<testCnt; i++)
	{
		if (i == testCnt-1)
		{
			auto begin = chrono::high_resolution_clock::now();
			kEvent.tryWait(5000ms);
			auto end = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::microseconds>(end - begin);
			cout << "TestEventFunc2" << "tryWait time:"<< duration.count()<< endl;
		}
		else
		{
			kEvent.Wait();
			cout << "TestEventFunc2 Wait" << endl;
		}
		
	}
}

int main()
{
    std::thread testTh1(TestEventFunc1);
    std::thread testTh2(TestEventFunc2);

	int cinCnt{0};
	char inputCmd {'R'};
	cin.get(inputCmd);
	while ('q'!=inputCmd && cinCnt<testCnt*2)
	{
		if ('s' == inputCmd)
		{
			kEvent.SetEvent();
			cinCnt++;
		}
		cin.get(inputCmd);
	}

   
	if (testTh1.joinable())
	{
		testTh1.join();
	}

	if (testTh2.joinable())
	{
		testTh2.join();
	}

	std::cout << "test set event and wait_for event\n";
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


