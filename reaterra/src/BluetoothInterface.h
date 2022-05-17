#ifndef SRC_BLUETOOTHINTERFACE_H_
#define SRC_BLUETOOTHINTERFACE_H_

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <gio/gio.h>

/*! @public
 * @enum BleObjectType
 * Перечисление типов объектов стека BLE.
 */
enum class BleObjectType {
	Adapter,		//!< Хост-адаптер.
	Device,			//!< Клиентское устройство.
	Service,		//!< Сервис на клиентском устройстве.
	Characteristic,	//!< Характеристика сервиса.
	Descriptor		//!< Дескриптор.
};


/// Объект BLE стека. Базовый класс.
struct BleObject {
	BleObjectType type; //!< Тип объекта.
	std::string path;	//!< Путь к объекту (его адрес в иерархии).

	/*!
	 * @brief Конструктор.
	 *
	 * @param t тип объекта.
	 * @param p путь к объекту.
	 */
	BleObject(BleObjectType t, std::string p)
	: type{t}, path{p}
	{}

	//! Деструктор.
	virtual ~BleObject() = default;
};


/// Характеристика.
struct BleCharacteristic : BleObject {
	std::string uuid;		//!< Идентификатор.
	std::string service;	//!< Сервис к которому принадлежит характеристика.
	std::function<void(std::vector<unsigned char>)> on_notify;	//!< Функция вызываемая при изменении характеристики.

	/*!
	 * @brief Конструктор.
	 *
	 * @param _path путь к объекту.
	 * @param _uuid идентификатор объекта.
	 * @param _service сервис к которому принадлежит характеристика.
	 */
	BleCharacteristic(std::string _path, std::string _uuid, std::string _service)
		: BleObject(BleObjectType::Characteristic, _path),
		  uuid {_uuid},
		  service {_service} {}
};


/// Сервис.
struct BleService : BleObject {
	std::string uuid;	//!< Идентификатор.
	std::string device;	//!< Устройство к которому принадлежит сервис.
	std::vector<std::shared_ptr<BleCharacteristic>> characteristics;	//!< Список характеристик сервиса.


	/*!
	 * @brief Конструктор.
	 *
	 * @param _path путь к объекту.
	 * @param _uuid идентификатор объекта.
	 * @param _device устройство к которому принадлежит сервис.
	 */
	BleService(std::string _path, std::string _uuid, std::string _device)
		: BleObject(BleObjectType::Service, _path),
		  uuid {_uuid},
		  device {_device} {}
};


/// Устройство.
struct BleDevice : BleObject {
	std::string address;	//!< Уникальный BLE адрес устройства.
	std::string name;		//!< Имя устройства.
	bool connected;			//!< Флаг состояния подключения.
	std::vector<std::shared_ptr<BleCharacteristic>> characteristics;	//!< Список характеристик устройства.
	std::vector<std::shared_ptr<BleService>> services;					//!< Список сервисов устройства.

	/*!
	 * @brief Конструктор.
	 *
	 * @param _path путь к устройству.
	 * @param _address BLE адрес устройства.
	 * @param _name имя устройства.
	 * @param _connected (опционально) флаг состояния подключения.
	 */
	BleDevice(std::string _path, std::string _address = {}, std::string _name = {}, bool _connected = false)
		: BleObject(BleObjectType::Device, _path),
		  address {_address},
		  name {_name},
		  connected{false} {}
};

typedef std::shared_ptr<BleDevice> BleDeviceSP;	//!< Умный указатель на объект BLE интерфейса.
typedef std::vector<BleDeviceSP> BleDeviceList;	//!< Умный указатель на список объектов BLE интерфейса.


/// Класс драйвера BLE интерфейса.
class BluetoothInterface {

public:

	//TODO: Перенести m_devices в приват и добавить нужные интерфейсы
	BleDeviceList m_devices;	//!< Список устройств.

	/*!
	 * @brief Конструктор.
	 *
	 * @param on_connect функция вызываемая при подключении устройства.
	 * @param on_disconnect функция вызываемая при отключении устройства.	 */
	BluetoothInterface(std::function<void (BleDeviceSP)> on_connect = {},
			std::function<void (BleDeviceSP)> on_disconnect = {});

	/// Деструктор.
	virtual ~BluetoothInterface();

	/*!
	 * @brief Запуск сканирования.
	 *
	 * @param on_device_found (опционально) функция вызываемая при обнаружении устройства.
	 */
	void startScan(std::function<void (BleDeviceSP)> on_device_found = {});

	/// Остановка сканирования.
	void stopScan();

	/*!
	 * @brief Подключение к устройству.
	 *
	 * @param address BLE адрес устройства.
	 * @return 0: успех, -1: неудача.
	 */
	int connect(std::string address);

	/*!
	 * @brief Отключение устройства.
	 *
	 * @param d отключаемое устройство.
	 */
	void disconnect(BleDeviceSP d);

	/*!
	 * @brief Включение отправки уведомлений характеристики на устройстве.
	 *
	 * @param d устройство.
	 * @param uuid идентификатор характеристики.
	 * @param on_notify (опционально) функция вызываемая при получении уведомления.
	 */
	void startNotify(BleDeviceSP d, std::string uuid, std::function<void (std::vector<unsigned char>)> on_notify = {});

	/*!
	 * @brief Чтение значения характеристики.
	 *
	 * @param d устройство.
	 * @param ch идентификатор характеристики.
	 * @return значение характеристики.
	 */
	std::vector<unsigned char> readCharacteristic(BleDeviceSP d, std::string ch);

	/*!
	 * @brief Добавление устройства в список.
	 *
	 * @param device добавляемое устройство.
	 */
	void addDevice(BleDeviceSP device);

	/// Удаление всех устройств из списка.
	void removeDevices();

	/*!
	 * @brief Определение необходимости выполнять сканирование.
	 *
	 * @return true: сканирование необходимо.
	 */
	bool needScan();

	/*!
	 * @brief Обработчик событий DBus
	 *
	 * @param connection
	 * @param sender_name
	 * @param object_path
	 * @param interface_name
	 * @param signal_name
	 * @param parameters
	 * @param user_data	 */
	void onSignal(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path,
			const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data);


private:

	int dev_id;
	int sock;
	std::thread m_glib_thread;
	std::atomic_bool m_running;
	std::thread m_thread;
	GMainLoop* m_loop;
	GDBusConnection *m_dbus_conn;
	std::mutex m_mutex;
	bool m_init_ok;
	GDBusProxy *m_adapter_proxy;
	std::function<void (BleDeviceSP)> m_on_device_found;
	std::function<void (BleDeviceSP)> m_on_connect;
	std::function<void (BleDeviceSP)> m_on_disconnect;
	std::string m_chosen_device;
	bool m_connected;
	std::string m_adapter_path;
	std::map<std::string, BleDeviceSP> m_device_cache;
	bool m_need_scan;

	void glib_worker();
	void worker();
	int bleConnect(std::string addr);
	int bleSend(std::string addr, std::string parameter, std::string value);
	void printTypeOfVariant(GVariant *v);
	std::shared_ptr<BleObject> sendInfoRequest(std::string path);
	int notify(std::string addr, std::string parameter);
	void onInterfaceAdded(GVariant* parameters);
	void onPropertiesChanged(std::string path, GVariant* parameters);
	BleDeviceSP parseDevice(GVariant* dev_var);
	std::shared_ptr<BleObject> parseIntrospectable(GVariant* var);
	std::vector<unsigned char> readCharacteristic(std::shared_ptr<BleCharacteristic> ch);
	BleDeviceList getCachedDevices();

};


#endif /* SRC_BLUETOOTHINTERFACE_H_ */
