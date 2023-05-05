/*
  ���� windows ϵͳ�е� Event �¼�, ���� C++ ��׼��ʵ��һ�����ƵĹ��ܡ�
  Ŀ���������߳�������ͨѶ
  ���ߣ� ������
  ���䣺 forwaradapeng@gmail.com
*/



#ifndef	_cplusplus_event_h_
#define	_cplusplus_event_h_ 
#include <mutex>
#include <chrono>

class Event
{
public:
	enum class Mode: uint8_t
	{
	   // ��ĳһ���ȴ��� Wait() ���Զ�����
	   Auto,
	   
	   //��Ҫͨ�� Reset() �ֶ�����, ���߱�����Ϊ���źź�һֱ�������ź�״̬
	   //���е� Wait() �������̷���
	   Manual
	};

public:
	explicit Event(Mode mode = Mode::Auto, bool initstate = false);
	~Event() = default;
	
	//�����¼����ź�
	inline void SetEvent()   const;
	
	//�����¼�Ϊ���ź�
	inline void ResetEvent() const;
	
	//�����ȴ�ֱ���¼����ź�, ����� Auto ģʽ�����ȴ������߳����õ�״̬
	inline void Wait()       const;	
	
	//�ȴ��¼�һ��ʱ��, ʱ�䷶Χ���¼����ź��򷵻� true,���򷵻� false
	template <typename Rep, typename Period>
	inline bool tryWait(const std::chrono::duration<Rep, Period>& duratio) const;

	//��鵱ǰ�߳��Ƿ����ź�, ֻ�۲첻�ı��κ�״̬
	inline bool isSignal()const;
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

Event::Event(Mode mode, bool initstate)
	:m_event(std::make_shared<EventImpl>(mode, initstate))
{}

void Event::SetEvent() const
{
	m_event->Set();
}

void Event::ResetEvent() const
{
    m_event->Reset();
}

void Event::Wait() const
{
	m_event->Wait();
}

template <typename Rep, typename Period>
bool Event::tryWait(const std::chrono::duration<Rep, Period>& duratio) const
{
	return m_event->Wait_for(duratio);
}

bool Event::isSignal()const
{
	std::unique_lock<std::mutex> r_lock(m_event->m_mutex);
	return m_event->m_signal;
}

Event::EventImpl::EventImpl(Mode mode, bool initState)
	:m_mode(mode),
	 m_signal(initState)
{}

void Event::EventImpl::Set()
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

void Event::EventImpl::Wait()
{
	std::unique_lock<std::mutex> r_lock(m_mutex);
	m_condi.wait(r_lock,[this]{ return m_signal;});
	if (Mode::Auto == m_mode)
	{
		m_signal = false;
	}

	return;
}

void Event::EventImpl::Reset()
{
	std::unique_lock<std::mutex> w_lock(m_mutex);
	m_signal = false;
}

template <typename Rep, typename Period>
bool Event::EventImpl::Wait_for(const std::chrono::duration<Rep, Period>& duration)
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
