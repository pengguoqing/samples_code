
#ifndef __cxx_refcounter_h__
#define __cxx_refcounter_h__

#include <atomic>
class  CEXRefCounter
{
public:

	//�޲������죬������ʼ��Ϊ0
	CEXRefCounter():m_count(0) {};

	//��ֹ��ʽ�Զ�ת��Ϊ CEXRefCounter
	explicit CEXRefCounter(int init_value) :m_count(init_value) {};

	//��������	
	CEXRefCounter(const CEXRefCounter& another)
		:m_count(another.value())
	{}

	//û�б�¶����Դ��ָ�룬��������ֱ��Ĭ��
	~CEXRefCounter() = default;

	//��ֵ���������
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

	//ת��Ϊ int ����
	operator int() const
	{
		return m_count.load(std::memory_order_relaxed);
	};

	// ��ȡ��ǰ����
	int value() const
	{
		return m_count.load(std::memory_order_relaxed);
	}

	// ǰ ++
	int operator ++ ()
	{
		return ++m_count;
	};

	// �� ++
	int operator ++ (int)
	{
		return m_count++;
	};

	//ǰ--
	int operator -- ()
	{
		return --m_count;
	};

	// ��--
	int operator -- (int)
	{
		return m_count--;
	};

	// ���� if ��ʹ�õ��߼���
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