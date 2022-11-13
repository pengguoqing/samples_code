/*
  仿照 windows 系统中的 Event 事件, 利用 C++ 标准库实现一个类似的功能。
  目标是用于线程任务间的通讯
  作者： 彭国庆
  邮箱： forwaradapeng@gmail.com
*/



#ifndef	_cplusplus_event_h
#define	_cplusplus_event_h 
#include <mutex>
#include <chrono>

class CXEvent
{
public:
	enum class Mode: uint8_t
	{
	   // 被某一个等待者 Wait() 后自动重置
	   Auto,
	   
	   //需要通过 Reset() 手动重置, 否者被设置为有信号后一直处于有信号状态
	   //所有的 Wait() 都能立刻返回
	   Manual
	};

public:
	explicit CXEvent(Mode mode = Mode::Auto, bool initstate = false);
	~CXEvent() = default;
	
	//设置事件有信号
	inline void SetEvent()   const;
	
	//重置事件为无信号
	inline void ResetEvent() const;
	
	//持续等待直到事件有信号, 如果是 Auto 模式则必须等待到该线程能拿到状态
	inline void Wait()       const;	
	
	//等待事件一定时间, 时间范围内事件有信号则返回 true,否则返回 false
	template <typename Rep, typename Period>
	inline bool tryWait(const std::chrono::duration<Rep, Period>& duratio) const;

private:
	struct EventImpl
	{
		explicit EventImpl(Mode mode, bool initState);
		~EventImpl() = default;
		inline void  Set();
		inline void  Reset();
		inline void  Wait();

		template <typename Rep, typename Period>
		inline bool Wait_for(const std::chrono::duration<Rep, Period>& duration);

		std::mutex				m_mutex;
		std::condition_variable m_condi;
		bool					m_signal;
		const Mode				m_mode;
	};

private:
	const std::shared_ptr<EventImpl> m_event;
};

CXEvent::CXEvent(Mode mode, bool initstate)
	:m_event(std::make_shared<EventImpl>(mode, initstate))
{}

void CXEvent::SetEvent() const
{
	m_event->Set();
}

void CXEvent::ResetEvent() const
{
    m_event->Reset();
}

void CXEvent::Wait() const
{
	m_event->Wait();
}

template <typename Rep, typename Period>
bool CXEvent::tryWait(const std::chrono::duration<Rep, Period>& duratio) const
{
	return m_event->Wait_for(duratio);
}

CXEvent::EventImpl::EventImpl(Mode mode, bool initState)
	:m_mode(mode),
	 m_signal(initState)
{}

void CXEvent::EventImpl::Set()
{
	{
		std::unique_lock<std::mutex> w_lock(m_mutex);
		if (m_signal)
		{
			return;
		}
		m_signal = true;
	}

	if (Mode::Auto == m_mode)
	{
		m_condi.notify_one();
	}
	else
	{
		m_condi.notify_all();
	}
}

void CXEvent::EventImpl::Wait()
{
	std::unique_lock<std::mutex> r_lock(m_mutex);
	m_condi.wait(r_lock,[this]{ return m_signal;});
	if (Mode::Auto == m_mode)
	{
		m_signal = false;
	}

	return;
}
void CXEvent::EventImpl::Reset()
{
	std::unique_lock<std::mutex> w_lock(m_mutex);
	m_signal = false;
}

template <typename Rep, typename Period>
bool CXEvent::EventImpl::Wait_for(const std::chrono::duration<Rep, Period>& duration)
{
	std::unique_lock<std::mutex> r_lock(m_mutex);
	if (!m_condi.wait_for(r_lock, duration, [this] { return m_signal; }))
	{
		return false;
	}

	if (Mode::Auto == m_mode)
	{
		m_signal = false;
	}

	return true;
}

#endif // _cplusplus_event_h
