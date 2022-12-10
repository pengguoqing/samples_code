#ifndef __cxx_shared_ptr_h__
#define __cxx_shared_ptr_h__
#include "refcounter.h"
#include <functional>

template<class T>
struct CXRelease
{
	void operator()(T* data) noexcept
	{
		delete data;
	}
};

template<class T>
struct CXReleaseArray
{
	void operator()(T* data) noexcept
	{
		delete[] data;
	}
};

// T:模板资源类, Func:自定义资源释放
template<class T, class Destutct = CXRelease<T>>
class CEXSharedPtr
{
	using Function = std::function<void(T*)>;

public:
	inline CEXSharedPtr()
			:m_ref(nullptr),
			 m_data(nullptr) 
	{}

	inline CEXSharedPtr(T* ptr)
			:m_ref(ptr ? new CEXRefCounter(1) : nullptr),
			 m_data(ptr),
			 m_deleter(Destutct())
	{}

	inline CEXSharedPtr(T* ptr, CEXRefCounter* ref)
		: m_ref(ref),
		  m_data(ptr),
		  m_deleter(Destutct())
	{
		if (m_ref) m_ref->Duplicate();
	}

	//注册类似 lambda表达式 的仿函数析构器
	template<class T, class LambdaDestruct>
	inline CEXSharedPtr(T* ptr, LambdaDestruct&& lambda)
			:m_ref(ptr ? new CEXRefCounter(1) : nullptr),
			 m_data(ptr),
		     m_deleter(lambda)
	{}

	////注册函数析构器
	//template<class T, class FuncDestruct>
	//inline CEXSharedPtr(T* ptr, FuncDestruct&& func)
	//	   :m_ref(ptr ? new CEXRefCounter : nullptr),
	//		m_data(ptr),
	//		m_deleter(std::forward<FuncDestruct>(func))
	//{}

	inline CEXSharedPtr(const CEXSharedPtr& another)
			:m_data(another.m_data),
			 m_ref(another.m_ref),
			 m_deleter(another.m_deleter)
	{
		     if (m_ref) m_ref->Duplicate();
	}

	inline CEXSharedPtr(CEXSharedPtr&& another) noexcept
		   :m_data(another.m_data),
			m_ref(another.m_ref),
		    m_deleter(another.m_deleter)
	{
		another.m_ref  = nullptr;
		another.m_data = nullptr;
	}

	inline ~CEXSharedPtr()
	{
		Release();
	}

	CEXSharedPtr& operator = (const CEXSharedPtr& another)
	{
		return Assign(another);
	}

	CEXSharedPtr& operator = (T* data)
	{
		return Assign(data);
	}

	CEXSharedPtr& operator = (CEXSharedPtr&& another) noexcept
	{
		Release();
		m_data = another.m_data;
		m_ref  = another.m_ref;
        
		another.m_data = another.m_ref = nullptr;
	}

	T* Get() const noexcept
	{
		return m_data;
	}

	T* operator -> () const noexcept
	{
		return m_data;
	}

	T& operator * () const noexcept
	{
	   return  *m_data;
	}

	explicit operator bool() const noexcept
	{
		return nullptr != m_data;
	}

	bool operator !() const noexcept
	{
		return nullptr == m_data;
	}

	bool operator == (const T* data) const
	{
		return Get() == data;
	}

	bool operator == (const CEXSharedPtr& another) const
	{
		return Get() == another->Get();
	}

	int Count() const
	{
		return m_ref ? m_ref->value() : 0;
	}


	CEXSharedPtr& Assign(T* data)
	{
		if (Get() != data)
		{
			CEXSharedPtr tmp(data);
			Swap(tmp);
		}

		return *this;
	}
	
	CEXSharedPtr& Assign(const CEXSharedPtr& another)
	{
		if (&another != Get())
		{
			CEXSharedPtr tmp(another);
			Swap(tmp);
		}

		return *this;
	}

	void Reset()
	{
		Assign(nullptr);
	}

	void Reset(T* data)
	{
		Assign(data);
	}

	void Reset(const CEXSharedPtr& another)
	{
		Assign(another);
	}

	template<class Other>
	CEXSharedPtr<Other> Cast() const
	{
		Other* ptr = dynamic_cast<Other*>(m_data);
		if (ptr)
		{
			return CEXSharedPtr<Other>(ptr, m_ref);
		}
		else
		{
			return CEXSharedPtr<Other>();
		}
	}
private:
	void Release()
	{
		if (m_ref &&  0 >= m_ref->Release())
		{
			m_deleter(m_data);
			m_data = nullptr;
			
			delete m_ref;
			m_ref = nullptr;	
		}
	}
	void Swap(CEXSharedPtr& another) noexcept
	{
		std::swap(this->m_data, another.m_data);
		std::swap(this->m_ref, another.m_ref);
	}


private:
	CEXRefCounter* m_ref;
	T*			   m_data;
	Function       m_deleter;
};

template <class T, class... Args>
CEXSharedPtr<T> MakeShared(Args&&... args)
{
	return CEXSharedPtr<T>(new T{std::forward<Args>(args)...});
}

template <class T>
CEXSharedPtr<T> MakeArrayShared(int size)
{
	return CEXSharedPtr<T>(new T[size], CXReleaseArray<T>());
}
#endif