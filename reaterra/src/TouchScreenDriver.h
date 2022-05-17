#ifndef TOUCHSCREENDRIVER_H_
#define TOUCHSCREENDRIVER_H_

#include <atomic>
#include <thread>

/// Драйвер сенсорной панели.
class TouchScreenDriver {

public:

	/// Конструктор.
	TouchScreenDriver();

	/// Деструктор.
	virtual ~TouchScreenDriver();

private:

	struct Slot {
		bool touched;
		int x;
		int y;
	};



	static const int NUM_OF_EVENTS = 5;
	static const int EPOLL_TIMEOUT_MS = 1000;

	std::atomic_bool m_running;
	std::thread m_thread;
	int m_fd;
	Slot m_slots[10];

	void worker();
	int openEvent();
	int make_socket_non_blocking(int sfd);

};

#endif /* TOUCHSCREENDRIVER_H_ */
