
#include <iostream>
#include <string>
#include "singleton.h"

struct A
{
    A(int data, std::string str)
        :m_data(data),
         m_str(str)
    {
     std::cout<<"A construct"<<std::endl;
    };

    ~A()
    {
        std::cout << "A deconstruct" << std::endl;
    }

    void Dump()
    {
        std::cout<<"A-data: "<< m_data <<" A-str: " << m_str << std::endl;
    }
 int m_data;
 std::string m_str;
};

struct B
{
	B(std::string&& rvalue)
	{
      std::cout<<"B construct"<< std::endl;
    };

	~B()
	{
		std::cout << "B deconstruct" << std::endl;
	};

    void Print()
    {
        std::cout << "dummy class B" << std::endl;
    }
};

int main(int argc, char*argv[])
{
    int data = 100;
    A* ptr = nullptr;
    std::string str ="Hello world!";
    ptr = CXSingleton<A>::InitInstance(data, str);
    CXSingleton<A>::GetInstance()->Dump();
    CXSingleton<A>::DestoryInstance();
    ptr = CXSingleton<A>::GetInstance();
    if (nullptr == ptr)
    {
        std::cout<<"A instance destroy"<<std::endl<<"------------"<<std::endl;
    }

    CXSingleton<B>::InitInstance(std::move(str));
    CXSingleton<B>::GetInstance()->Print();
    CXSingleton<B>::DestoryInstance();

    return 0;
}

