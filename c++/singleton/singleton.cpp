//#include "singleton.h"
//
//template<class T>
//std::atomic<T*> CXSingleton<T>::m_instance;
//
//template<class T>
//std::mutex CXSingleton<T>::m_mutex;
//
//template<class T>
//template<class... Args>
//static T* CXSingleton<T>::InitInstance(Args&&... args)
//{
//	T* ins = m_instance.load(std::memory_order_relaxed);
//	std::atomic_thread_fence(std::memory_order_acquire);
//	if (nullptr==ins) 
//	{
//		std::lock_guard<std::mutex> w_lock(m_mutex);
//		ins = m_instance.load(std::memory_order_relaxed);
//		if (nullptr==ins) {
//			ins = new T(std::forward<Args>(args)...);
//			std::atomic_thread_fence(std::memory_order_release);
//			m_instance.store(ins, std::memory_order_relaxed);
//		}
//	}
//	return ins;
//
//}
//
//template<class T>
//T* CXSingleton<T>::GetInstance()
//{
//	return m_instance.load(std::memory_order_relaxed);
//}
//
//template<class T>
//void CXSingleton<T>::DestoryInstance()
//{
//	delete m_instance.load(std::memory_order_relaxed);
//}
//
//template class CXSingleton<int>;