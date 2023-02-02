#include<iostream>
#include<string>
#include<vector>

using namespace std;

namespace PX_CPLUSPLUS
{
    class CXRAII
    {
    public:
        CXRAII(size_t size =0, char ch = 'a')
        :m_datasize(size),
         m_dataptr(size ? new char[size]{ch} : nullptr)
        {
            cout<<"construct"<<endl;
        }
    
        ~CXRAII()
        {
            delete[] m_dataptr;
            m_datasize = 0;
            cout<<"deconstruct"<<endl;
        }
    
        CXRAII (const CXRAII& another)
        :m_datasize(another.m_datasize),
         m_dataptr(another.m_datasize ? new char[another.m_datasize]{} : nullptr)
        {
            std::copy(another.m_dataptr, another.m_dataptr+m_datasize, m_dataptr);
            cout<<"copy construct"<<endl;
        }

        CXRAII(CXRAII&& another) noexcept
        //:CXRAII()
        {
            this->m_dataptr  = nullptr;
            this->m_datasize = 0;

            Swap(another);
            cout<<"move construct"<<endl;
        }


        // CXRAII& operator=(const CXRAII &another)
        // {
        //     if(this != &another)
        //     {
        //         delete[] m_dataptr;
    
        //         m_dataptr  = another.m_datasize ? new char[another.m_datasize]{} : nullptr;
        //         m_datasize = another.m_datasize;
        //         std::copy(another.m_dataptr, another.m_dataptr+m_datasize, m_dataptr);
        //     }
    
        //     return *this;
        // }

        // CXRAII& operator=(const CXRAII &another)
        // {
        //     if(this != &another)
        //     {
        //         char*  dataptr  = another.m_datasize ? new char[another.m_datasize]{} : nullptr;
        //         size_t datasize = another.m_datasize;
        //         std::copy(another.m_dataptr, another.m_dataptr+datasize, dataptr);

        //         delete[] m_dataptr;
        //         m_dataptr   = dataptr;
        //         m_datasize  = datasize;      
                
        //     }
    
        //     return *this;
        // }
        
        CXRAII& operator=(CXRAII another) noexcept
        {
            cout<<"operatro = "<<endl;
            Swap(another);
            return *this;
        }

    public:
       void Dump()
       {
            cout<<m_datasize<<endl;
            if (m_dataptr)
            {
               cout<<*m_dataptr<<endl;
            }
       }

       friend void Swap(CXRAII& first, CXRAII& second);

    private:
        void Swap(CXRAII & another) noexcept
        {
            std::swap(this->m_dataptr,  another.m_dataptr);
            std::swap(this->m_datasize, another.m_datasize);
        }
    
    private:
       char*   m_dataptr;
       size_t  m_datasize;
    };


    void Swap(CXRAII& first, CXRAII& second)
    {
        first.Swap(second);
    }
}

PX_CPLUSPLUS::CXRAII GetRAII()
{
    return PX_CPLUSPLUS::CXRAII(3, 'c');
}

int main(int argc, char** argv)
{

    cout<<"default fun test========="<<endl;    
    PX_CPLUSPLUS::CXRAII resman1(1);
    resman1.Dump();

    PX_CPLUSPLUS::CXRAII resman2(2, 'b');
    resman2.Dump();

    resman2 = resman1;
    resman2 = GetRAII();
    PX_CPLUSPLUS::Swap(resman1, resman2);
    resman1.Dump();
    resman2.Dump();

    cout<<"container test========="<<endl;  

    vector<PX_CPLUSPLUS::CXRAII> resmans(2);
    cout<<resmans.capacity()<<endl;
    resmans.emplace_back();
    //resmans.push_back(PX_CPLUSPLUS::CXRAII());       
    cout<<resmans.capacity()<<endl;
    
    cout<<"test end ========="<<endl;

    return 0;
}       