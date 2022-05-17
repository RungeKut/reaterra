#ifndef SYNCQUEUE_H_
#define SYNCQUEUE_H_

#include <condition_variable>
#include <chrono>
#include <memory>
#include <mutex>
#include <deque>


template <typename To, typename From, typename Deleter>
    std::unique_ptr<To, Deleter> dynamic_unique_cast(std::unique_ptr<From, Deleter>&& p) {
        if (To* cast = dynamic_cast<To*>(p.get()))
        {
            std::unique_ptr<To, Deleter> result(cast, std::move(p.get_deleter()));
            p.release();
            return result;
        }
        return std::unique_ptr<To, Deleter>(nullptr); // or throw std::bad_cast() if you prefer
    }

/// Синхронизованная очередь.
template <class A> class SyncQueue {

public:
	/// Конструктор.
	SyncQueue(std::string m_name = "");
	/// Деструктор.
	virtual ~SyncQueue();

	/*!
	 * @brief Поместить событие в конец очереди.
	 *
	 * @param event событие.
	 */
	void putEvent(A event);

	/*!
	 * @brief Поместить событие в начало очереди.
	 *
	 * @param event событие.
	 */
	void putAsNext(A event);

	/*!
	 * @brief Получить событие из очереди.
	 *
	 * @return событие.
	 */
	A getEvent();

	/*!
	 * @brief Получить размер очереди.
	 *
	 * @return размер очереди.
	 */
	int getSize();

	/*!
	 * @brief очистить очередь.
	 */
	void clear();

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::deque<A> m_queue;
	std::string m_name;
	std::string m_type_name;
};

#include "Logger.h"

template <class A> SyncQueue<A>::SyncQueue(std::string m_name)
:	m_name {m_name},
	m_type_name {"SyncQueue " + m_name}
{
	LDt(m_type_name, "Ctor queue name: " + m_name);
}

template <class A> SyncQueue<A>::~SyncQueue() {
	LDt(m_type_name, "Dtor");
}

template <class A> void SyncQueue<A>::putEvent(A event) {
//	LDt(m_type_name, "In put event");
	std::unique_lock<std::mutex> lock {m_mutex};
	m_queue.push_back(std::move(event));
	lock.unlock();
	m_cv.notify_one();
}

template <class A> void SyncQueue<A>::putAsNext(A event) {
	std::unique_lock<std::mutex> lock {m_mutex};
	m_queue.push_front(std::move(event));
	lock.unlock();
	m_cv.notify_one();
}


template <class A> A SyncQueue<A>::getEvent() {
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_queue.empty()) {
		m_cv.wait_for(lock, std::chrono::milliseconds(1000));
	}

	auto ret = std::move(m_queue.front());
	m_queue.pop_front();

	return ret;
}


template <class A> int SyncQueue<A>::getSize() {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.size();
}

template <class A> void SyncQueue<A>::clear() {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.clear();
}

#endif /* SYNCQUEUE_H_ */
