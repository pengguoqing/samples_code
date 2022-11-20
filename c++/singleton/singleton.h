
/*
作用: C++ 单例模式的懒汉版本, 支持泛型和线程安全创建资源
作者: Guoqing Peng
邮箱: forwardapeng@gmail.com
*/

#pragma once

#include <atomic>
#include <mutex>

template <class T>
class CXSingleton
{
public:

	template<class... Args>
	static T* InitInstance(Args&&... args)
	{
		T* ins = m_instance.load(std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_acquire);
		if (nullptr == ins)
		{
			std::lock_guard<std::mutex> w_lock(m_mutex);
			ins = m_instance.load(std::memory_order_relaxed);
			if (nullptr == ins) {
				ins = new T(std::forward<Args>(args)...);
				std::atomic_thread_fence(std::memory_order_release);
				m_instance.store(ins, std::memory_order_relaxed);
			}
		}
		return ins;
	}

	static T* GetInstance()
	{
		return m_instance.load(std::memory_order_relaxed);
	}

	static void DestoryInstance()
	{
		delete m_instance.load(std::memory_order_relaxed);
		m_instance.store(nullptr, std::memory_order_relaxed);
	}

private:
	CXSingleton()  = default;
	~CXSingleton() = default;
	CXSingleton(const CXSingleton& another) = delete;
	CXSingleton& operator=(const CXSingleton& another) = delete;

private:
	static std::atomic<T*> m_instance;
	static std::mutex m_mutex;
};

template<class T>
std::atomic<T*> CXSingleton<T>::m_instance;

template<class T>
std::mutex CXSingleton<T>::m_mutex;
//#include "singleton.cpp"
