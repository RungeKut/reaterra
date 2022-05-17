#include "HrMonitor.h"

#include <algorithm>

#include "Context.h"
#include "Utils.h"

HrMonitor::HrMonitor()
:	m_monitor_path {ctx.m_db.getParam("СенсорЧСС")},
	m_battery {-1.},
	m_hrate {0.},
	m_ble {
		[this](BleDeviceSP d)
			{
				LD("Connect to " + d->address + " : " + d->name);

				if (!std::any_of(d->characteristics.begin(), d->characteristics.end(), [](auto c){return c->uuid.find("00002a37-") == 0;})) {
					LD("Invalid device");
					m_connect_addr = "";
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::HrMonitorEvent, "BAD DEVICE"));
					m_ble.disconnect(d);
					return;
				}

				m_hr_monitor = d;
				m_connected = true;
				m_ble.startNotify(d, "00002a37-0000-1000-8000-00805f9b34fb", [this](auto d)
					{
						LD("on notify: " + Utils::bytes2Hex(d.data(), d.size()));
						if (d[1]) {
							m_hrate = d[1];
						}
						ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
					});
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::HrMonitorEvent, "CONNECTED"));
				if (m_monitor_path.empty()) {
					m_monitor_path = d->path;
					ctx.m_db.setParam("СенсорЧСС", d->path);
					ctx.m_db.setParam("СенсорЧССАдрес", d->address);
					ctx.m_db.setParam("СенсорЧССНазвание", d->name);
				}
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
			},
		[this](BleDeviceSP d)
			{
				LD("Disconnect from " + d->address + " : " + d->name);
				m_hr_monitor = nullptr;
				m_connected = false;
				m_battery = -1;
				m_hrate = 0;
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::HrMonitorEvent, "DISCONNECTED"));
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
				if (!m_monitor_path.empty()) {
					m_connect_addr = m_monitor_path;
					m_scan_flag = true;
					m_scan_counter = 0;
				}
			}
	},
	m_running {true},
	//TODO: Использовать базовый класс ThreadClass для обработчика
	m_thread (&HrMonitor::worker, this),
	m_scan_flag {false},
	m_scan_counter {0},
	m_connected {false}
{
	LD("Ctor");

	if (!m_monitor_path.empty()) {
		BleDeviceSP device = std::make_shared<BleDevice>(m_monitor_path, ctx.m_db.getParam("СенсорЧССАдрес"), ctx.m_db.getParam("СенсорЧССНазвание"));
		m_ble.addDevice(device);
		connect(m_monitor_path);
	}

}

HrMonitor::~HrMonitor() {
	m_running = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
	LD("Dtor");
}

void HrMonitor::connect(std::string path, bool rescan) {
	m_connect_addr = path;
	if (m_ble.needScan() && rescan) {
		m_scan_flag = true;
	} else {
//		LD("connect to " + path);
		if (m_ble.connect(path)) {
//			LD("conect failed");
			if (rescan) {
//			LD("Remove devices and rescan");
			m_ble.removeDevices();
			m_scan_flag = true;
			}
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::HrMonitorEvent, "CONNECTION_ERROR"));
		}
	}
}

void HrMonitor::disconnect() {
	if (isConnected()) {
		m_ble.disconnect(m_hr_monitor);
	}
}

float HrMonitor::getBattery() {
	return m_battery;
}

float HrMonitor::getHRate() {
	return m_hrate;
}

bool HrMonitor::isConnected() {
	return m_connected;
}

void HrMonitor::startDiscovery() {
	disconnect();
	m_ble.m_devices.clear();
	m_ble.startScan([this](BleDeviceSP device)	{
			LD("device found " + device->path);
			m_ble.addDevice(device);
		});
}

void HrMonitor::stopDiscovery() {
	m_ble.stopScan();
}

void HrMonitor::worker() {
	int delay = DELAY_TIME_MS;
	while (m_running) {
		if (isConnected()) {
			// Если устройство подключено, получить данные уровня батареи
//			LT("before read hr battery");
			std::vector<unsigned char> data = m_ble.readCharacteristic(m_hr_monitor, "00002a19-0000-1000-8000-00805f9b34fb");
			if (!data.empty()) {
//				LT("set hr battery");
				m_battery = data.at(0);
			}
			// Задержка между запросами
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		} else if (m_scan_flag && !m_scan_counter && !m_connect_addr.empty()) {
			// Включить сканирование если есть адрес устройства и установлены условия сканирования - флаг и счетчик
			m_scan_counter++;
//			LD("start scan");
			m_ble.startScan([this](BleDeviceSP device)	{
//					LD("device found " + device->path);
					if (device->path == m_connect_addr) {
						// Если обнаружено нужное устройство, добавить его в список устройств и завершить сканирование
						m_ble.addDevice(device);
						m_ble.stopScan();
						// Задержка для завершения остановки сканирования
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//						LD("try to connect " + m_connect_addr);
						// Попытка установить соединение с найденным устройством
						connect(m_connect_addr, false);
					}
				});
			// Задержка перед следующим циклом
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		} else if (m_scan_flag && m_scan_counter++ > 20 && !m_connect_addr.empty()) {
			// Отключить сканирование после 20 * 400мс если устройства не найдены
			m_scan_flag = false;
			m_scan_counter = 0;
			m_ble.stopScan();
			if (m_monitor_path.empty()) {
				// Если поиск был по запросу UI, то отправить событие для вывода сообщения.
				// Если поиск фоновый - при установленном в БД адресе устройства, то сообщение не выводится
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::HrMonitorEvent, "CONNECTION_ERROR"));
			}
			// Задержка перед следующим циклом
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		} else if (!m_monitor_path.empty() && !m_scan_flag && m_scan_counter++ > 10) {
			// Если в БД установлен адрес устройства, нет подключения и остановлен поиск в течении 10 * DELAY_TIME_MS,
			// то выполнить попытку подключиться
			m_scan_counter = 0;
			connect(m_monitor_path);
			// Задержка перед следующим циклом
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		} else {
			// Задержка перед следующим циклом
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
	}
}

std::shared_ptr<BleDevice> HrMonitor::getDeviceAt(unsigned i) {
	if (m_ble.m_devices.size() > i) {
		return m_ble.m_devices.at(i);
	} else {
		return nullptr;
	}
}

unsigned HrMonitor::getDeviceListSize() {
	return m_ble.m_devices.size();
}

void HrMonitor::clearStoredDevice() {
	m_monitor_path.clear();
	ctx.m_db.setParam("СенсорЧСС", m_monitor_path);
	ctx.m_db.setParam("СенсорЧССАдрес", "");
	ctx.m_db.setParam("СенсорЧССНазвание", "");
}
