#ifndef SRC_TITABITINTERFACE_H_
#define SRC_TITABITINTERFACE_H_

#include <thread>
#include <atomic>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>

#include "SyncQueue.h"
#include "Timer.h"


/// Интерфейс с визуализатором от Titabit - планшет.
class TitabitInterface  {


public:

	/// Конструктор.
	TitabitInterface();

	/// Деструктор.
	virtual ~TitabitInterface();

	/// Инициализация.
	void init();

	/// Отправка телеметрии на планшет.
	void sendSpeed(int speed);



private:

	static const int EXO_LINE_MAX = 2000;
	static const int TB_CONNECTION_LOOP_TIMEOUT_MS = 900;
	static const int TB_HB_SEND_DELAY_MS = 5000;
	const int CONFIG_PORT;

	/// Флаг true - планшет подключен.
	std::atomic_bool m_is_connected;
	int m_socket_desc;
	int m_client;
	std::thread m_thread;
	std::thread m_write_thread;
	std::thread m_connection_loop_thread;
	std::atomic_bool m_running;
	std::ofstream m_os;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic_int m_new_client;
	std::atomic_bool m_worker_sleeping;
	struct sockaddr_in m_client_addr;
	std::chrono::steady_clock::time_point m_last_send_time;
	std::chrono::steady_clock::time_point m_start_time;
	SyncQueue<std::string> m_queue;
	Timer m_timer;

	void worker();
	void write_worker();
	void connectionLoop();
	void sendHB();
	void disconnect();
	void sendToClient(std::string data);

};

#endif /* SRC_TITABITINTERFACE_H_ */
