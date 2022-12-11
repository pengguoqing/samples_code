
#ifndef __cxx_refcounter_h__
#define __cxx_refcounter_h__

#include <atomic>
class  CEXRefCounter
{
public:

	//无参数构造，计数初始化为0
	CEXRefCounter():m_count(0) {};

	//禁止隐式自动转换为 CEXRefCounter
	explicit CEXRefCounter(int init_value) :m_count(init_value) {};

	//拷贝构造	
	CEXRefCounter(const CEXRefCounter& another)
		:m_count(another.value())
	{}

	//没有暴露的资源类指针，析构函数直接默认
	~CEXRefCounter() = default;

	//赋值运算符重载
	CEXRefCounter& operator = (const CEXRefCounter& another)
	{
		m_count.store(another.value());
		return *this;
	};
	CEXRefCounter& operator = (int value)
	{
		m_count.store(value);
		return *this;
	};

	//转换为 int 类型
	operator int() const
	{
		return m_count.load(std::memory_order_relaxed);
	};

	// 获取当前计数
	int value() const
	{
		return m_count.load(std::memory_order_relaxed);
	}

	// 前 ++
	int operator ++ ()
	{
		return ++m_count;
	};

	// 后 ++
	int operator ++ (int)
	{
		return m_count++;
	};

	//前--
	int operator -- ()
	{
		return --m_count;
	};

	// 后--
	int operator -- (int)
	{
		return m_count--;
	};

	// 重载 if 中使用的逻辑符
	operator bool() const
	{
		return 0 != m_count.load();
	};

	int Release()
	{
		return --m_count;
	}

	void AddRef()
	{
		m_count++;
	}

private:
	std::atomic<int> m_count;
};

#endif 