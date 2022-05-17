#ifndef WIFIREMOTE_H_
#define WIFIREMOTE_H_

#include <thread>
#include <atomic>
#include <sys/socket.h> // socket
#include <arpa/inet.h>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "SoundInterface.h"

/// Класс поддержки работы с планшетом.
class WiFiRemote {

public:

	/// Конструктор.
	WiFiRemote();

	/// Деструктор.
	virtual ~WiFiRemote();

	/*!
	 * @brief Отправка данных на планшет.
	 *
	 * @param data данные.
	 * @param id идентификатор пакета.
	 */
	void send(std::vector<unsigned char> &data, int id);

	/*!
	 * @brief Проверка подключения.
	 *
	 * @return true: подключен, false: не подключен.
	 */
	bool isConnected();

private:

	const static int DESK_PORT = 5000;
	const static int MONITOR_CONNECTION_LOOP_TIMEOUT_MS = 1000;
	const static int READ_TIMEOUT_MS = 1000;
	const static int HB_TIMEOUT_MS = 6000;

	int m_client;
	int m_socket_desc;
	std::atomic_bool m_running;
	std::atomic_int m_new_client;
	std::atomic_bool m_worker_sleeping;
	struct sockaddr_in m_client_addr;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::thread m_thread;
	std::thread m_connection_loop_thread;
	std::vector<char> m_buff;
	std::chrono::steady_clock::time_point m_last_receive_time;
	int m_sent_packet_id;
	int m_conf_packet_id;


	void worker();
	void connectionLoop();
	void disconnect();
	void process(std::string data);
	void readRemote();

};

#endif /* WIFIREMOTE_H_ */
