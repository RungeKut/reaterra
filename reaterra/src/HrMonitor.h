#ifndef HRMONITOR_H_
#define HRMONITOR_H_

#include "BluetoothInterface.h"

/// Драйвер монитора пульса.
class HrMonitor {

public:

	/// Конструктор.
	HrMonitor();

	/// Деструктор.
	virtual ~HrMonitor();

	/*!
	 * brief Подключение устройства.
	 *
	 * @param path адрес устройства.
	 * @param rescan выполнение предварительного сканирования (true: да, false: нет).
	 */
	void connect(std::string path, bool rescan = true);

	/// Отключение подключенного устройства.
	void disconnect();

	/// Получение уровня заряда батареи.
	float getBattery();

	/// Получение значения пульса.
	float getHRate();

	/*!
	 * brief Получение состояния подключения устройства.
	 *
	 * @return true: подключено, false: отключено.
	 */
	bool isConnected();

	/// Начать поиск устройств.
	void startDiscovery();

	/// Остановить поиск устройств.
	void stopDiscovery();

	/*!
	 * @brief Получение устройства из списка обнаруженных.
	 *
	 * @param i индекс.
	 * @return ссылка на устройство.
	 */
	std::shared_ptr<BleDevice> getDeviceAt(unsigned i);

	/*!
	 * @brief Получение размера списка устройств.
	 *
	 * @return размер списка.
	 */
	unsigned getDeviceListSize();

	/// Сброс сохраненного в БД устройства.
	void clearStoredDevice();

	//TODO: Перенести m_monitor_path в приват.
	/// Адрес текущего установленного устройства.
	std::string m_monitor_path;


private:

	static const int DELAY_TIME_MS = 400;

	std::shared_ptr<BleDevice> m_hr_monitor;
	float m_battery;
	float m_hrate;
	BluetoothInterface m_ble;
	std::atomic_bool m_running;
	std::thread m_thread;
	bool m_scan_flag;
	int m_scan_counter;
	std::string m_connect_addr;
	std::atomic_bool m_connected;

	void worker();

};

#endif /* HRMONITOR_H_ */
