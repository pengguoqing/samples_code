

#include <iostream>
#include <memory>
#include "shared_ptr.h"

void CustomDeleter(int* data)
{
    delete data;
}

struct CXCustomDelete
{
    void operator()(int* data)
    {
        delete data;
    }
};

struct CXSuper
{
    CXSuper()
    {
        std::cout<< "CXSuper construct"<<std::endl;
    }
    virtual ~CXSuper()
    {
        std::cout << "CXSuper destruct" << std::endl;
    }
};

struct CXSub : public CXSuper
{
    CXSub()
	{
		std::cout << "CXSub construct" << std::endl;
	}
	~CXSub()
	{
		std::cout << "CXSub destruct" << std::endl;
	}
    
    int m_data{10};
};


struct CXMembers
{
    CXMembers(char ch='c', int in=0, float fl=.0f)
        :m_char(ch),
         m_inte(in),
         m_float(fl)
    {}
    char  m_char;
    int   m_inte;
    float m_float;
    
};
int main()
{
    CEXSharedPtr<int> ptr(new int{0});
    std::cout<< "ptr" <<"count: "<<ptr.Count()<<"  data: "<<*ptr<<std::endl;
    
    CEXSharedPtr<int> ptr1(new int{1}, [](int* ptr){delete ptr;});
    std::cout << "ptr1" << "count: " << ptr1.Count() << "  data: " << *ptr1 << std::endl;
    
    std::function<void(int*)> deleter(CustomDeleter);
    CEXSharedPtr<int> ptr2(new int{2}, deleter);
    std::cout << "ptr2" << "count: " << ptr2.Count() << "  data: " << *ptr2 << std::endl;

    CEXSharedPtr<int> ptr3(std::move(ptr2));
    ptr2.Count();
    std::cout << "ptr3" << "count: " << ptr3.Count() << "  data: " << *ptr3 << std::endl;

    CEXSharedPtr<int> ptr4(ptr3);
    std::cout << "ptr4" << "count: " << ptr4.Count() << "  data: " << *ptr4 << std::endl;

    CEXSharedPtr<CXSuper> superPtr(new CXSub);
    CEXSharedPtr<CXSub> subPtr(superPtr.Cast<CXSub>());

    CEXSharedPtr<CXMembers> initListPtr = MakeShared<CXMembers>('I', 1, .5f);

    CEXSharedPtr<CXMembers> arrayPtr =    MakeArrayShared<CXMembers>(5);
    std::cout << "Hello World!\n";
}

