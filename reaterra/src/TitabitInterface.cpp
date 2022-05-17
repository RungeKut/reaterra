#include "TitabitInterface.h"

#include "Logger.h"
#include <sstream>
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <cstring> // strcpy, strcat, strlen
#include <iomanip>
#include <netinet/tcp.h>
#include <regex>

#include "Context.h"
#include "PersistedParameter.h"
#include "Utils.h"



TitabitInterface::TitabitInterface()
	: CONFIG_PORT(ctx.m_settings.getParamInt("titabit_port")),
	  m_is_connected(false),
	  m_running(true),
	  m_new_client(-1),
	  m_worker_sleeping(false),
	  m_timer {[this](int id) {
			static int cur_speed = 0;
			if (cur_speed != ctx.m_treadmill.getActualSpeed()) {
				cur_speed = ctx.m_treadmill.getActualSpeed();
				sendSpeed(cur_speed);
			}
		}, 0}

{
	LD("Ctor");
	m_client = -1;
	m_socket_desc = -1;
	m_last_send_time = std::chrono::steady_clock::now();
	m_start_time = std::chrono::steady_clock::now();
	init();
	m_timer.startTimerMillis(500);
}

TitabitInterface::~TitabitInterface() {
    m_running = false;


    if(m_thread.joinable()) {
    	m_thread.join();
    }

    sendHB();
    m_queue.putEvent("");
    if(m_write_thread.joinable()) {
    	m_write_thread.join();
    }

    if (m_connection_loop_thread.joinable()) {
    	m_connection_loop_thread.join();
    }

	LD("Dtor");
}

void TitabitInterface::init() {
	LI("Interface started");

    //Create socket
    m_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_desc == -1) {
        LE("ERROR! Could not create interface socket. Error: " + std::to_string(errno));
        return;
    }

    //Prepare the sockaddr_in structure
    struct sockaddr_in server;
 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = INADDR_ANY;
 	server.sin_port = htons(CONFIG_PORT);

 	int on = 1;
 	if (setsockopt(m_socket_desc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
 		LE("ERROR! Could not set socket option SO_REUSEADDR. Error: " + std::to_string(errno));
 		return;
 	}

 	if (setsockopt(m_socket_desc, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0) {
 		LE("ERROR! Could not set socket option TCP_NODELAY. Error: " + std::to_string(errno));
 		return;
 	}

 	//Bind
 	if (bind(m_socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
 		LE("ERROR! Could not bind configuration socket. Error: " + std::to_string(errno));
 		return;
 	}

	//Accept incoming connection
	if (listen(m_socket_desc , 3) < 0) {
 		LE("ERROR! Socket listen error: " + std::to_string(errno));
		return;
	}

	m_connection_loop_thread = std::thread(&TitabitInterface::connectionLoop, this);
    m_thread = std::thread(&TitabitInterface::worker, this);
    m_write_thread = std::thread(&TitabitInterface::write_worker, this);
	LI("Interface initialized");
}

void TitabitInterface::worker() {
    while(m_running){
    	if (m_new_client == -1) {
    		if (m_client == -1) {
    			m_worker_sleeping = true;
    		    std::unique_lock<std::mutex> lk(m_mutex);
    		    m_cv.wait(lk);
    			m_worker_sleeping = false;
    		} else {
    			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
    															- m_last_send_time).count() > TB_HB_SEND_DELAY_MS)
    			{
    	    		m_last_send_time = std::chrono::steady_clock::now();
					sendHB();
    			}
    		}
    	} else {
    		if (m_client != -1) {
    			LI("Disconnected by new connection in worker");
				disconnect();
    		}
    		m_last_send_time = std::chrono::steady_clock::now();
            m_client = m_new_client;
            m_new_client = -1;

            std::string addr(inet_ntoa(m_client_addr.sin_addr));
            std::stringstream ss;
            ss << "Client connected from " << addr << ":" << m_client_addr.sin_port;
            LI(ss.str());
            m_is_connected = true;
            m_queue.clear();
            sendToClient("DEV=REATERRA");
            sendToClient("VER=2");
            sendSpeed(ctx.m_treadmill.getActualSpeed());
    	}
    }
 	LI("Interface stopped");
}


void TitabitInterface::connectionLoop() {
	LI("Waiting for incoming connections...");
	struct sockaddr_in client_addr;
    while(m_running){

    	fd_set set;
        struct timeval tv;

        //Accept connection from an incoming client
		tv.tv_sec = (int)(TB_CONNECTION_LOOP_TIMEOUT_MS  / 1000);
		tv.tv_usec = (TB_CONNECTION_LOOP_TIMEOUT_MS % 1000) * 1000;
		FD_ZERO(&set);
		FD_SET(m_socket_desc, &set);

		//Receive a message from client
		int err = select(m_socket_desc + 1, &set, NULL, NULL, &tv);
		int client = -1;
		if (err > 0) {
			socklen_t c = sizeof(client_addr);
			client = accept(m_socket_desc, (struct sockaddr *)&client_addr, &c);
			if (client < 0) {
				LE("ERROR! Accepting of the pad client connection is failed. Error: " + std::to_string(errno));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				continue;
			}
		} else if (err == 0) {
	           continue;
		} else {
			LE("Server socket select error: " + std::to_string(errno));
		}

		if (m_new_client != -1) {
			LI("Disconnected by new connection in connection loop");
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
    LD("Exit from connectionLoop");
}



void TitabitInterface::disconnect() {
	if (m_os.is_open()) {
		m_os.close();
	}

	close(m_client);
	m_client = -1;
	LI("Client disconnected");
    m_is_connected = false;
}

void TitabitInterface::sendSpeed(int speed) {
	sendToClient("SPD=" + std::to_string(ctx.m_treadmill.getActualSpeed() / 2));
}

void TitabitInterface::sendToClient(std::string msg) {
	if (m_is_connected) {
		char client_message[EXO_LINE_MAX];
		int timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start_time).count();
		strcpy(client_message, (std::to_string(timestamp) + ":" +  msg + "\n").c_str());
		LD("send: " + client_message);
		m_queue.putEvent(client_message);
	}
}


void TitabitInterface::write_worker() {
	LD("write worker started");

    while(m_running) {
		std::string str = m_queue.getEvent();

		if (!m_is_connected || !m_running) {
			continue;
		}
		int err = send(m_client , str.c_str() , str.size(), MSG_NOSIGNAL);
		if (str.back() == '\n') {
			str.pop_back();
		}
		if (err < 0) {
			LI("Disconnected by send error: " + std::to_string(errno));
			disconnect();
		}
		m_last_send_time = std::chrono::steady_clock::now();

    }

	LD("write worker stopped");
}


void TitabitInterface::sendHB() {
	// HeartBeats are not used still
}

