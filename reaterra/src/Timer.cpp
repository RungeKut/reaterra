#include "Timer.h"

#include <sstream>

#include "Context.h"

Timer::Timer(TimerFunc on_timer, int id)
:	m_running {true},
	m_timeout {0},
	m_id {id},
	m_ontimer {on_timer}
{
	const void * address = static_cast<const void*>(this);
	std::stringstream ss;
	ss << address;
	m_this_addr = ss.str();
	LD("Ctor ID:" + std::to_string(m_id) + "  [" + m_this_addr + "]");
	m_thread = std::thread(&Timer::worker, this);
}

Timer::~Timer() {
	m_running = false;
	m_timeout = 1;
	m_cv.notify_all();
	if (m_thread.joinable()) {
		m_thread.join();
	}

	LD("Dtor ID:" + std::to_string(m_id) + "  [" + m_this_addr + "]");
}

void Timer::startTimerMillis(int delay) {
	m_timeout = delay;
	m_cv.notify_all();
}

void Timer::stopTimer() {
	m_timeout = 0;
}

void Timer::worker() {
	while (m_running) {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (m_running && !m_timeout) {
			m_cv.wait_for(lock, std::chrono::milliseconds(1000));
		}

		lock.unlock();

		while (m_running && m_timeout) {
			//TODO: Timer shouldn't depends on custom procedure delay
			std::this_thread::sleep_for(std::chrono::milliseconds(m_timeout));
			if (m_running && m_timeout) {
				m_ontimer(m_id);
			}
		}
	}
}
