#include "TouchScreenDriver.h"

#include <sys/epoll.h> 		// epoll_create1 epoll_ctl epoll_wait
#include <fcntl.h>
#include <linux/input.h> // EVIOCGNAME
#include <unistd.h>   // close
#include <cstring> 			// strerror

#include <chrono>

#include "Context.h"
#include "TouchEvent.h"
#include "Utils.h"

TouchScreenDriver::TouchScreenDriver()
:	m_running {true},
	//TODO: Использовать базовый класс ThreadClass для обработчика
	m_thread (&TouchScreenDriver::worker, this),
	m_fd {-1}
{
	LD("Ctor");

 }

TouchScreenDriver::~TouchScreenDriver() {
	m_running = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}

	if (m_fd != -1) {
		close(m_fd);
	}

	LD("Dtor");
}

void TouchScreenDriver::worker() {
	while (m_running) {
		if (openEvent()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			continue;
		}

		struct epoll_event ep_input = { 0 };
		int fd_ep = -1;

		fd_ep = epoll_create1(0);
		if (fd_ep < 0) {
			LE("Error creating epoll");
			return;
		}

		ep_input.data.fd = m_fd;
		ep_input.events = EPOLLIN | EPOLLET;
		if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, m_fd, &ep_input) < 0) {
			LE("Fail to add fd to epoll");
			return;
		}

		std::string event;
		int fingers = 0;
		std::string fin = "CLEAN";
		std::string cur_fin = "CLEAN";

		int slot = 0;
		int cur_slot = 0;
		int track = 0;
		int x = 0;
		int y = 0;

		while (m_running) {
//			int fin_pev = fingers;


//			LD("Wait events");
			int fdcount;
			struct epoll_event ev[NUM_OF_EVENTS];
			fdcount = epoll_wait(fd_ep, ev, NUM_OF_EVENTS, EPOLL_TIMEOUT_MS);
//			LD("fdcount = " + std::to_string(fdcount));;
			if (fdcount < 0) {
				if (errno != EINTR)
					LE("Error receiving input message: " + strerror(errno));
				continue;
			}
			for (int i = 0; i < fdcount; i++) {
				if (ev[i].data.fd == m_fd ) {
			        const size_t ev_size = sizeof(struct input_event);
			        ssize_t size = 0;

			        struct input_event evt;

			        while ((size = read(m_fd, &evt, ev_size)) > 0) {
						if (size < static_cast<int>(ev_size)) {
							LD("Error size when reading");
							close(m_fd);
							m_fd = -1;
							break;
						}

						if (evt.type == EV_SYN) {
//							LD("Event: " + event);
							cur_slot = 0;
							track = 0;

							if (event.find(" ABS [TRACK:-1]") == 0 || event.find(" ABS [SLOT:0] ABS [TRACK:-1]") == 0) {
								fin = "CLEAN";
								fingers = 0;
							}
							event = "";
						} else if (evt.type == EV_ABS) {
							event += " ABS ";
							if (evt.code == ABS_X) {
								event += "[X:" + std::to_string(evt.value) + "]";
							} else if (evt.code == ABS_Y) {
								event += "[Y:" + std::to_string(evt.value) + "]";
							} else if (evt.code == ABS_PRESSURE) {
								event += "[PRESS:" + std::to_string(evt.value) + "]";
							} else if (evt.code == ABS_MT_SLOT) {
								event += "[SLOT:" + std::to_string(evt.value) + "]";
								cur_slot = evt.value;
//								if (fin == "ONE") {
//									fingers++;
//								}
							} else if (evt.code == ABS_MT_TRACKING_ID) {
								event += "[TRACK:" + std::to_string(evt.value) + "]";
								track = evt.value;
								if (evt.value == static_cast<int>(0xFFFFFFFF)) {
//									fingers--;
								}
							} else if (evt.code == ABS_MT_POSITION_X) {
								event += "[MT_X:" + std::to_string(evt.value) + "]";
								if (track != static_cast<int>(0xFFFFFFFF)) {
									if (fingers == 0) {
										fingers++;
										fin = "ONE";
										slot = cur_slot;
									} else if (slot != cur_slot) {
										fin = "MULTI";
									}
									x = evt.value;
								}
							} else if (evt.code == ABS_MT_POSITION_Y) {
								event += "[MT_Y:" + std::to_string(evt.value) + "]";
								if (track != static_cast<int>(0xFFFFFFFF)) {
									if (fingers == 0) {
										fingers++;
										fin = "ONE";
										slot = cur_slot;
									} else if (slot != cur_slot) {
										fin = "MULTI";
									}
									y = evt.value;
								}
							} else {
								event += "[UNK:" + Utils::int2Hex(evt.code, 4) + "]";
							}
						} else if (evt.type == EV_KEY) {
							event += " KEY ";
							event += " UNK:" + Utils::int2Hex(evt.code, 4);
			        	} else {
							event += " UNK " + Utils::int2Hex(evt.type, 4);
							event += " UNK:" + Utils::int2Hex(evt.code, 4);
			        	}
//						printf("%010d.%03d Type: %04X   Code:%04X    Value:%08X\n", evt.time.tv_sec, evt.time.tv_usec / 1000, evt.type,
//								evt.code, evt.value);
//						fflush(stdout);
//						LD(event);
			        }

				}
			}

			std::string adds;
			if (fin != cur_fin) {

				if (!ctx.m_remote.isConnected()) {
					if (fin == "ONE") {
						adds = " X:" + std::to_string(x) + " Y:" + std::to_string(y);
						ctx.m_queue.putEvent((std::unique_ptr<Event>)std::make_unique<TouchEvent>(TouchEvent::TouchType::Touch, x, y));
					} else if (fin == "CLEAN") {
						if (cur_fin == "ONE") {
						adds = " X:" + std::to_string(x) + " Y:" + std::to_string(y);
						ctx.m_queue.putEvent((std::unique_ptr<Event>)std::make_unique<TouchEvent>(TouchEvent::TouchType::Untouch, x, y));
						} else {
							ctx.m_queue.putEvent((std::unique_ptr<Event>)std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
						}
					} else if (fin == "MULTI") {
						ctx.m_queue.putEvent((std::unique_ptr<Event>)std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
					}
					LD("Fingers: " + fin + adds);
				}

				cur_fin = fin;
			}
		}
	}
}

int TouchScreenDriver::openEvent() {
	std::string event_file = ctx.m_settings.getParamString("touch_path");
	m_fd = open(event_file.c_str(), O_RDONLY);
	if (m_fd == -1) {
		return -1;
	}

	/* Print Device Name */
    char name[256] = "Unknown";
	int ret = ioctl(m_fd, EVIOCGNAME(sizeof(name)), name);
	if (ret < 0) {
		close(m_fd);
		m_fd = -1;
		return -1;
	}

	LI("Found input device: " + name);
	make_socket_non_blocking(m_fd);
	return 0;

}


int TouchScreenDriver::make_socket_non_blocking(int sfd)
{
        int flags, s;

        flags = fcntl(sfd, F_GETFL, 0);
        if (flags == -1) {
                perror("fcntl");
                return -1;
        }

        flags |= O_NONBLOCK;
        s = fcntl(sfd, F_SETFL, flags);
        if (s == -1) {
                perror("fcntl");
                return -1;
        }

        return 0;
}

