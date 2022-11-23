#ifndef _cplusplus_task_h_
#define _cplusplus_task_h_

#include <functional>

class CXTask
{
public:
	using Function = std::function<void()>;

public:
	inline CXTask();
	inline CXTask(const CXTask& another);
	inline CXTask(CXTask&& another) noexcept;
	inline CXTask& operator=(const CXTask& another);
	inline CXTask& operator=(CXTask&& another);

	inline CXTask(const Function& func);
	inline CXTask(Function&& func);
	inline CXTask& operator=(Function&& func);
	inline CXTask& operator=(const Function& func);

	inline void operator() () const;
	inline operator bool() const;

private:
	Function m_func;
};

CXTask::CXTask() 
{}

CXTask::CXTask(const Function& func)
	:m_func(func)
{}

CXTask::CXTask(Function&& func)
	:m_func(std::move(func))
{}


CXTask::CXTask(const CXTask& another)
	:m_func(another.m_func)
{}

CXTask::CXTask(CXTask&& another) noexcept
	:m_func(std::move(another.m_func))
{}

CXTask& CXTask::operator=(const CXTask& another)
{
	m_func = another.m_func;
	return *this;
}

CXTask& CXTask::operator=(CXTask&& another)
{
	m_func = std::move(another.m_func);
	return *this;
}

CXTask& CXTask::operator=(Function&& func)
{
	m_func = std::move(func);
	return *this;
}

CXTask& CXTask::operator=(const Function& func)
{
	m_func = func;
	return *this;
}

void CXTask::operator()() const
{
	m_func();
}

CXTask::operator bool() const
{
	return m_func.operator bool();
}

//任意类型函数和参数
template<class Function, class... Args>
inline CXTask CreateTask(Function&& func, Args&&... args)
{
	return CXTask(std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
}

//函数对象, 比如 lambda
template<class Function>
inline CXTask CreateTask(Function&& func)
{
	return CXTask(std::forward<Function>(func));
}

#endif 
