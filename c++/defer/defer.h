#ifndef _cplusplus_defer_h_
#define _cplusplus_defer_h_

/*
	内容: C++ 实现 Go语言里面 defer 类似的功能, 以 defer() 宏函数的方式对外提供使用
	作用: 延迟执行当前作用域内的 defer() 声明, 当离开作用域时所有的 defer() 声明都会被执行
		  通常用于一次性清理资源, RAII 的衍生品

	作者: 彭国庆
	邮箱: forwardapeng@gmail.com
	example:
	{
	  defer(cout<<"world! ");
	  cout<<"Hello "<<endl;
	}
*/

template<class Functor>
class CXDeferImpl
{
public:
	inline CXDeferImpl(const Functor& functor);
	inline CXDeferImpl(Functor&& functor);
	inline CXDeferImpl(CXDeferImpl&& another);
	inline ~CXDeferImpl();

private:
	//不允许拷贝, 赋值
	CXDeferImpl(const CXDeferImpl& another)			  = delete;
	CXDeferImpl operator=(const CXDeferImpl& another) = delete;
	CXDeferImpl operator=(CXDeferImpl&& another)	  = delete;

private:
	Functor  m_func;
	bool     m_valid;
};


template<class Functor>
inline CXDeferImpl<Functor>::CXDeferImpl(const Functor& functor)
	:m_func(functor)
	,m_valid(true)
{}

template<class Functor>
inline CXDeferImpl<Functor>::CXDeferImpl(Functor&& functor)
	:m_func(std::move(functor))
	,m_valid(true)
{}

template<class Functor>
inline CXDeferImpl<Functor>::CXDeferImpl(CXDeferImpl&& another)
	:m_func(std::move(another.m_func))
	,m_valid(true)
{
	another.m_valid = false;
}

template<class Functor>
inline CXDeferImpl<Functor>::~CXDeferImpl()
{
	if (m_valid)
	{
		m_func();
	}
}

template<class F>
CXDeferImpl<F> MakeDeferIns(F&& functor)
{
	return CXDeferImpl<F>(std::forward<F>(functor));
}


#define CONCAT_(a, b, c) a##b##c
#define CONCAT(a, b, c)  CONCAT_(a, b, c)

#define defer(x) \
  auto CONCAT(defer_, __LINE__, __COUNTER__) = MakeDeferIns([&]{x;})


#endif 