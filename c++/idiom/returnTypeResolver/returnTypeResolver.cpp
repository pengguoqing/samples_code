#include <iostream>
#include <list>
#include <vector>
#include <string>

using namespace std;


class CXFromString
{
public:
	CXFromString(const char* str)
		:m_str{str}
	{}

	operator int()
	{
        return std::stoi(m_str);

	}
    operator float()
    {
        return std::stof(m_str);
    }

private:
	const string m_str;
};


class CXRange
 {

public:
    CXRange(const size_t from, const size_t to): from{from}, to{to}
    { }

    template<typename Container>
    operator Container()
    {
        Container container;
        for(size_t i = from; i < to; ++i)
            container.insert(container.end(), i);
        return container;
    }
    
private:
    const size_t from, to;
};

int main(int argc, char** argv)
{
    int     valuei = CXFromString("666");
	float   valuef = CXFromString("6.66");
    cout<< valuei <<endl;
    cout<< valuef <<endl;
    
    std::vector<int> veci = CXRange(0, 3);
    for(const auto& item : veci)
        std::cout << item << std::endl;

    std::list<int> listf = CXRange(3, 6);
    for(const auto&item : listf)
        std::cout << item << std::endl;

    return 0;

}