#include "ThreadClass.h"

ThreadClass::ThreadClass()
:	m_running {true}
{
}

ThreadClass::~ThreadClass() {
	stop();
}

void ThreadClass::start() {
    m_thread = std::thread(&ThreadClass::worker, this);
}

void ThreadClass::worker() {

	while (m_running) {
		while (m_running && !init()) {}
		while (m_running && process()) {}
	}
}

void ThreadClass::stop() {
	m_running = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
}
