#ifndef SRC_DEVICEMONITOR_H_
#define SRC_DEVICEMONITOR_H_

#include <libudev.h>

#include <functional>
#include <thread>
#include <utility>
#include <vector>

#include "UsbFlashDrive.h"

/// Монитор USB устройств.
class DeviceMonitor {

public:

	/// Конструктор.
	DeviceMonitor();

	/// Деструктор.
	virtual ~DeviceMonitor();

	/*!
	 * @brief Получение списка USB устройств.
	 *
	 * @return список устройств.
	 */
	std::vector<UsbFlashDrive>& getUsbDrives();

	/*!
	 * @brief Установка функции обработки события.
	 *
	 * @param on_event функция вызываемая при подключении/отключении устройства.
	 */
	void setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)> on_event);

	/*!
	 * @brief Получение состояния проверки наличия USB устройств при старте программы.
	 *
	 * @return истина - устройства подключены.
	 */
	bool isDriveChecked();


private:

	static const int NUM_OF_EVENTS = 5;
	static const int EPOLL_TIMEOUT_MS = 1000;

	bool m_running;
	std::thread m_thread;
	struct udev *udev;
	std::vector<UsbFlashDrive> m_usb_drives;
	std::function<void(std::string type, UsbFlashDrive &drive)> m_on_usb_drive_event;
	bool m_drives_checked;

	void worker();
	void scanUsbFlashDrives();

};

#endif /* SRC_DEVICEMONITOR_H_ */
