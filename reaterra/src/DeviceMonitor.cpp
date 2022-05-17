#include "DeviceMonitor.h"

#include <sys/epoll.h> 		// epoll_create1 epoll_ctl epoll_wait

#include <cstring> 			// strerror
#include <algorithm>

#include <hidapi/hidapi.h>

#include "Context.h"
#include "GpioDriver.h"
#include "Logger.h"
#include "Utils.h"


DeviceMonitor::DeviceMonitor()
:	m_running {true},
	m_drives_checked{false}
{
	LD("Ctor");
	udev = udev_new();
	if (!udev) {
		LE("Error! Can't create udev");
		return;
	}
	//TODO: Использовать базовый класс ThreadClass для обработчика
	m_thread = std::thread(&DeviceMonitor::worker, this);
}

DeviceMonitor::~DeviceMonitor() {
	m_running = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
	udev_unref(udev);
	LD("Dtor");
}

std::vector<UsbFlashDrive>& DeviceMonitor::getUsbDrives() {
	return m_usb_drives;
}

void DeviceMonitor::worker() {
	LI("Device monitor is started");
	struct udev_device *dev;
	struct udev_device *dev_parent;
	int fd_udev = -1;
	struct epoll_event ep_udev = { 0 };
	struct udev_monitor *mon;
	int fd_ep = -1;


	int off_pin = ctx.m_settings.getParamInt("power_off_pin");

	if (off_pin) {
		GpioDriver::setPinDirection(off_pin, GpioDriver::PinDirection::In);
	}


	mon = udev_monitor_new_from_netlink(udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(mon, "block", NULL);
	udev_monitor_filter_add_match_subsystem_devtype(mon, "bluetooth", NULL);
	udev_monitor_enable_receiving(mon);

	/// Setup epoll
	fd_ep = epoll_create1(0);
	if (fd_ep < 0) {
		LE("Error creating epoll");
		return;
	}

	fd_udev = udev_monitor_get_fd(mon);
	ep_udev.events = EPOLLIN;
	ep_udev.data.fd = fd_udev;
	if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, fd_udev, &ep_udev) < 0) {
		LE("Fail to add fd to epoll");
		return;
	}

	scanUsbFlashDrives();
	m_drives_checked = true;

	bool halt = true;
	if (halt && m_usb_drives.size() && m_usb_drives.back().isValid() && m_usb_drives.back().isFileExisted("no_halt")) {
		halt = false;
	}

	while (m_running) {
		int fdcount;
		struct epoll_event ev[NUM_OF_EVENTS];
		int i = 0;

		if (off_pin && Context::main_board != "PI") {
			if (GpioDriver::getPinLevel(off_pin) == GpioDriver::PinLevel::Low) {
				if (halt) {
					LD("Halt executed");
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Shutdown));
				} else {
					LD("Halt blocked");
				}
			}
		}

		fdcount = epoll_wait(fd_ep, ev, NUM_OF_EVENTS, EPOLL_TIMEOUT_MS);
		if (fdcount < 0) {
			if (errno != EINTR)
				LE("Error receiving uevent message: " + strerror(errno));
			continue;
		}

		for (i = 0; i < fdcount; i++) {
			if (ev[i].data.fd == fd_udev && (ev[i].events & EPOLLIN)) {

				dev = udev_monitor_receive_device(mon);
				if (dev == NULL)
					continue;
				std::string dev_type {udev_device_get_subsystem(dev)};
				const char *devpath = udev_device_get_devpath(dev);
				if (!devpath) {
					devpath = "null";
				}
				const char *devnode = udev_device_get_devnode(dev);
				if (!devnode) {
					devnode = "null";
				}
				const char *syspath = udev_device_get_syspath(dev);
				if (!syspath) {
					syspath = "null";
				}
				LD("subsystem:" + dev_type + "  action:" + udev_device_get_action(dev) + "  devpath:" + devpath
						 + "  devnode:" + devnode + "  syspath:" + syspath);
				if (strcmp("add", udev_device_get_action(dev)) == 0) {

					std::string dev_driver {udev_device_get_sysname(dev) == NULL ? "" : udev_device_get_sysname(dev)};
					if (dev_driver.find("sd") == 0) {
						std::string dev_node {udev_device_get_devnode(dev)};
						auto drive = std::find_if(m_usb_drives.begin(), m_usb_drives.end(), [&dev_node](const auto d){return d.m_dev_node == dev_node;});
						UsbFlashDrive *drv = &*drive;
						if (drive == m_usb_drives.end()) {
							m_usb_drives.push_back(UsbFlashDrive(dev_node));
							drv = &m_usb_drives.back();
						} else {
							drv->m_valid = true;
						}
						LD("Usb flash drive was connected:" + dev_node);

						try {
							drv->driveMount();
							drv->driveUmount();
						} catch (...) {
							drv->m_valid = false;
						}

						if (drv->m_valid) {
							halt = !m_usb_drives.back().isFileExisted("no_halt");
							if (m_on_usb_drive_event) {
								m_on_usb_drive_event("add", *drv);
							}
						}
					}


					dev_parent = udev_device_get_parent(dev);
					dev_parent = udev_device_get_parent(dev_parent);
					if (dev_type == "tty") {
						dev_parent = udev_device_get_parent(dev_parent);
					}

					struct udev_list_entry *sysattr;
					std::string serial;
					std::string pid;
					std::string vid;

					udev_list_entry_foreach(sysattr, udev_device_get_sysattr_list_entry(dev_parent)) {
						const char *name;
						const char *value;

						name = udev_list_entry_get_name(sysattr);
						if (strcmp(udev_list_entry_get_name(sysattr), "serial") == 0) {
							value = udev_device_get_sysattr_value(dev_parent, name);
							if (value != NULL) {
								serial = value;
							}
						} else if (strcmp(udev_list_entry_get_name(sysattr), "idVendor") == 0) {
							value = udev_device_get_sysattr_value(dev_parent, name);
							if (value != NULL) {
								vid = value;
							}
						} else if (strcmp(udev_list_entry_get_name(sysattr), "idProduct") == 0) {
							value = udev_device_get_sysattr_value(dev_parent, name);
							if (value != NULL) {
								pid = value;
							}
						}
					}
				} else if (strcmp("remove", udev_device_get_action(dev)) == 0) {

					std::string dev_driver {udev_device_get_sysname(dev) == NULL ? "" : udev_device_get_sysname(dev)};
					if (dev_driver.find("sd") == 0) {
						std::string dev_node {udev_device_get_devnode(dev)};
						auto drive = std::find_if(m_usb_drives.begin(), m_usb_drives.end(), [&dev_node](const auto d){return d.m_dev_node == dev_node;});
						if (drive != m_usb_drives.end()) {
							(*drive).m_valid = false;
						}
						LD("Usb flash drive was disconnected:" + dev_node);
						halt = true;
						if (m_on_usb_drive_event) {
							m_on_usb_drive_event("remove", *drive);
						}
					}
				}
				udev_device_unref(dev);
			}
		}
	}
	LI("Device monitor is stopped");
}

void DeviceMonitor::setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)> on_event) {
	m_on_usb_drive_event = on_event;
}

void DeviceMonitor::scanUsbFlashDrives() {
	struct udev_enumerate *udev_enumerate;
	struct udev_list_entry *list_entry;

	udev_enumerate = udev_enumerate_new(udev);
	if (udev_enumerate == NULL) {
		LE("Error of udev_enumerate");
		return;
	}
	udev_enumerate_add_match_subsystem(udev_enumerate, "block");
	udev_enumerate_scan_devices(udev_enumerate);
	udev_list_entry_foreach(list_entry, udev_enumerate_get_list_entry(udev_enumerate)) {
		struct udev_device *device;

		device = udev_device_new_from_syspath(udev, udev_list_entry_get_name(list_entry));
		if (device != NULL) {
			std::string dev_type {udev_device_get_subsystem(device)};
			std::string dev_driver {udev_device_get_sysname(device) == NULL ? "" : udev_device_get_sysname(device)};
			if (dev_driver.find("sd") == 0) {
				std::string dev_node {udev_device_get_devnode(device)};
				auto drive = std::find_if(m_usb_drives.begin(), m_usb_drives.end(), [&dev_node](const auto d){return d.m_dev_node == dev_node;});
				if (drive == m_usb_drives.end()) {
					m_usb_drives.push_back(UsbFlashDrive(dev_node));
				} else {
					(*drive).m_valid = true;
				}
				LD("Usb flash drive was found:" + dev_node);
			}
			udev_device_unref(device);
		}
	}
	udev_enumerate_unref(udev_enumerate);
}

bool DeviceMonitor::isDriveChecked() {
	return m_drives_checked;
}
