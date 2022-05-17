#ifndef TIMER_H_
#define TIMER_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

/// Функция события таймера.
typedef std::function<void(int)> TimerFunc;


/// Реализация периодического таймера.
class Timer {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param on_timer функция обработчик события таймера.
	 * @param id идентификатор таймера.
	 */
	Timer(TimerFunc on_timer, int id = 0);

	/// Деструктор.
	virtual ~Timer();

	/*!
	 * @brief Запуск таймера.
	 *
	 * @param delay период таймара, мс.
	 */
	void startTimerMillis(int delay);

	/// Остановка таймера.
	void stopTimer();


private:

	std::atomic_bool m_running;
	std::thread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_timeout;
	int m_id;
	TimerFunc m_ontimer;
	std::string m_this_addr;


	void worker();

};

#endif /* TIMER_H_ */
