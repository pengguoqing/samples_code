// task.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>

#include "task.h"
#include "event.h"

using namespace  std;

queue<CXTask> g_tasklist;
CXEvent       g_task_notify;
CXEvent       g_quit_notify(CXEvent::Mode::Manual);

void ThreadFunc()
{
	while (!g_quit_notify.isSignal())
	{
		g_task_notify.Wait();
		if (g_tasklist.empty())
		{
			continue;
		}

		auto task = g_tasklist.front();
		if (task)
		{
			task();
		}
		g_tasklist.pop();
	}
}

void TaskFunc(char inputcmd)
{
	cout << "func task -- cur threadID:" << this_thread::get_id() << " --- keyWord is: " << inputcmd << endl;
}

int main()
{
    thread th1(ThreadFunc);
	thread th2(ThreadFunc);
	cout<<"th1 id: "<<th1.get_id()<< " th2 id: "<<th2.get_id() << endl;

	char inputCmd{ 'R' };
	cin.get(inputCmd);
	while ('q' != inputCmd)
	{
		//数字走 lambda创建 task
		if (inputCmd>='0'&& inputCmd <='9')
		{
			g_tasklist.push(CreateTask([inputCmd] {
				cout << "lambda task -- cur threadID:" << this_thread::get_id() << " --- keyWord is: " << inputCmd << endl;
				}));
		}
		
		//字母走 函数创建 task
		if (inputCmd >= 'a' && inputCmd <= 'z')
		{
			g_tasklist.push(CreateTask(TaskFunc, inputCmd));
		}

		g_task_notify.SetEvent();
		
		cin.clear();
		cin.ignore();
		cin.get(inputCmd);
	}
	
	g_quit_notify.SetEvent();

	if (th1.joinable())
	{
		th1.join();
	}

	if (th2.joinable())
	{
		th2.join();
	}
	   
	return 0;
}
