#include "WiFiRemote.h"

#include <poll.h> // poll, POLLIN
#include <unistd.h> // close
#include <sys/ioctl.h> // ioctl, FIONBIO
#include <sstream>
#include <cstring>
#include <netinet/tcp.h>

#include "Context.h"
#include "Logger.h"
#include "TouchEvent.h"
#include "Utils.h"


WiFiRemote::WiFiRemote()
: 	m_client {-1},
	m_socket_desc {-1},
	m_running {true},
	m_new_client {-1},
	m_sent_packet_id {0},
	m_conf_packet_id {0}
{
	LD("Dtor");

	m_buff.reserve(100);
	LI("WiFi interface started");
    //Create socket
    m_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_desc == -1) {
        LE("ERROR! Could not create desk WiFi interface socket");
        return;
    }
    LI("WiFi interface socket created");

    //Prepare the sockaddr_in structure
    struct sockaddr_in server {0};
 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = INADDR_ANY;
 	server.sin_port = htons(DESK_PORT);

 	int on = 1;
 	setsockopt(m_socket_desc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

 	// Set socket to be nonblocking
 	ioctl(m_socket_desc, FIONBIO, (char *)&on);

 	//Bind
 	if( bind(m_socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
 		LE("ERROR! Could not bind WiFi socket");
 		return;
 	}
 	LI("Bind WiFi socket done successfully");

	//Accept incoming connection
	//TODO: fail check for all functions can be failed
	listen(m_socket_desc , 3);

	m_connection_loop_thread = std::thread(&WiFiRemote::connectionLoop, this);
	//TODO: Использовать базовый класс ThreadClass для обработчика
    m_thread = std::thread(&WiFiRemote::worker, this);
}


WiFiRemote::~WiFiRemote() {
    m_running = false;
    m_cv.notify_all();

    if(m_thread.joinable()) {
    	m_thread.join();
    }

    if (m_connection_loop_thread.joinable()) {
    	m_connection_loop_thread.join();
    }
	LD("Dtor");
}


void WiFiRemote::worker() {
    while(m_running) {
    	if (m_new_client == -1) {
    		if (m_client == -1) {
    			m_worker_sleeping = true;
    		    std::unique_lock<std::mutex> lk(m_mutex);
    		    m_cv.wait(lk);
    			m_worker_sleeping = false;
    		} else {
    			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
    															- m_last_receive_time).count() > HB_TIMEOUT_MS)
    			{
    				LD("Try to disconnect by HB");
    				disconnect();
    			} else {
    				readRemote();
    			}
    		}
    	} else {
    		if (m_client != -1) {
				disconnect();
    		}
    		m_last_receive_time = std::chrono::steady_clock::now();

            std::string addr(inet_ntoa(m_client_addr.sin_addr));
            std::stringstream ss;
            ss << "WiFi client connected from " << addr << ":" << m_client_addr.sin_port;
            LI(ss.str());
            m_client = m_new_client;
            m_new_client = -1;
    	}
    }
 	LI("WiFi interface stopped");
}


void WiFiRemote::connectionLoop() {
	LI("Waiting for incoming connections...");
	struct sockaddr_in client_addr;
    while(m_running){

        struct pollfd fds[1];

        // Monitor socket for input
        fds[0].fd = m_socket_desc;
        fds[0].events = POLLIN;
        int err = poll(fds, 1, MONITOR_CONNECTION_LOOP_TIMEOUT_MS);

		int client = -1;
		if (err > 0) {
			socklen_t c = sizeof(client_addr);
			client = accept(m_socket_desc, (struct sockaddr *)&client_addr, &c);
			if (client < 0) {
				LE("ERROR! Accepting of the desk WiFi client connection is failed");
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				continue;
			}
			fcntl(client, F_SETFL, O_NONBLOCK);
//			int one = 1;
//		 	setsockopt(client, SOL_SOCKET, TCP_NODELAY, &one, sizeof(one));
		} else if (err == 0) {
	           continue;
		} else {
			LE("Server socket error");
		}

		if (m_new_client != -1) {
			disconnect();
			continue;
		}

		m_client_addr = client_addr;

		m_new_client = client;
        if (m_worker_sleeping) {
        	m_cv.notify_one();
        }
    }
    if (m_worker_sleeping) {
    	m_cv.notify_one();
    }
}


void WiFiRemote::send(std::vector<unsigned char>& data, int id) {
	if (m_conf_packet_id < m_sent_packet_id) {
		LD("PROBLEM: new id = " + std::to_string(m_conf_packet_id) + "    cur id = " + std::to_string(m_sent_packet_id));
//		if (m_sent_packet_id - m_conf_packet_id > 5) {
//			disconnect();
//			return;
//		}
	}
	m_sent_packet_id = id;
	LD("before send " + std::to_string(data.size()));
	if (m_client == -1) {
		LD("send o client");
		return;
	}


	int sent = 0;
    struct pollfd fds[1];

    // Мониторим готовность для вывода
    fds[0].fd = m_client;
    fds[0].events = POLLOUT;
	while (sent != static_cast<int>(data.size()) && m_client != -1) {
		int err = poll(fds, 1, READ_TIMEOUT_MS);

		if (err > 0) {
			int sz = ::send(m_client , data.data() + sent, data.size() - sent, 0);
			LD("sz = " + std::to_string(sz));
			if(sz < 0) {
				LD("send disconnect");
				disconnect();
				break;
			} else {
				sent += sz;
			}
		} else if (err == 0) {
			LD("send timeout");
		} else if (err < 0) {
			disconnect();
		}
	}

	LD("after send " + std::to_string(data.size()));
}


void WiFiRemote::readRemote() {
	char client_message[128];
    struct pollfd fds[1];

    // Monitor socket for input
    fds[0].fd = m_client;
    fds[0].events = POLLIN;
    int err = poll(fds, 1, READ_TIMEOUT_MS);

	if (err > 0) {
		int read_size = recv(m_client, client_message, sizeof(client_message), 0);
		if(read_size <= 0) {
			disconnect();
		} else {
			m_buff.insert(m_buff.end(), client_message, client_message + read_size);
			while (char *ptr  = (char *)memchr(m_buff.data(), 0, m_buff.size())) {
				if (ptr != nullptr) {
					int ind = ptr - m_buff.data();
//					LD("ind=" + std::to_string(ind));
					if (ind) {
						process(std::string {m_buff.data()});
						m_buff.erase(m_buff.begin(), m_buff.begin() + ind + 1);
					}
				}
//				LD("buff: [" + Utils::bytes2Hex(reinterpret_cast<unsigned char*>(m_buff.data()), m_buff.size()) + "]");
			}
		}
	} else if (err == 0) {
//		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
//													- m_last_receive_time).count() > HB_RECEIVE_TIMEOUT_MS) {
//			LI("TRACE disconnect by receive timeout");
//			disconnect();
//		}
	} else if (err < 0) {
		disconnect();
	}
}


void WiFiRemote::disconnect() {
	close(m_client);
	m_client = -1;
	m_conf_packet_id = m_sent_packet_id;
	LI("WiFi client disconnected");
}


bool WiFiRemote::isConnected() {
	return m_client == -1 ? false : true;
}


void WiFiRemote::process(std::string data) {
	LD("from remote:[" + data + "]");

	if (data.find("touch") != std::string::npos) {
		char action = data.at(6);
		int x = std::stoi(data.substr(8, 4));
		int y = std::stoi(data.substr(13, 4));

		if (action == 'D') {
			ctx.m_queue.putEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Touch, x, y));
		} else if (action == 'U') {
			ctx.m_queue.putEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Untouch, x, y));		}
	}

	unsigned pos = data.find(", id ");
	if (pos != std::string::npos) {
		m_conf_packet_id = std::stoi(data.substr(pos + 5, 10));
	}
	m_last_receive_time = std::chrono::steady_clock::now();
}
