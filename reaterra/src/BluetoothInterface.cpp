#include "BluetoothInterface.h"

#include <unistd.h>

#include <algorithm>
#include <chrono>

#include <glib.h>

#include "Context.h"
#include "Utils.h"

namespace {

	BluetoothInterface *btint = nullptr;

	static void dbusSignalHandler (GDBusConnection *connection, const gchar *sender_name, const gchar *object_path,
			const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data)
	{
		if (btint) {
			btint->onSignal(connection, sender_name, object_path, interface_name, signal_name, parameters, user_data);
		}
	}

}


BluetoothInterface::BluetoothInterface(std::function<void (BleDeviceSP)> on_connect,
		std::function<void (BleDeviceSP)> on_disconnect)
:	dev_id {-1},
	sock {-1},
	m_running {true},
	m_loop {nullptr},
	m_dbus_conn {nullptr},
	m_init_ok {false},
	m_adapter_proxy {nullptr},
	m_on_connect {on_connect},
	m_on_disconnect {on_disconnect},
	m_connected {false},
	m_need_scan {true}
{
	LD("Ctor");

	btint = this;

	try {
		// Запуск главного цикла gLib
		m_loop = g_main_loop_new(NULL, FALSE);
		m_glib_thread = std::thread(&BluetoothInterface::glib_worker, this);

		//TODO: Использовать базовый класс ThreadClass для обработчика
		// Запуск процесса обработки
		m_thread = std::thread(&BluetoothInterface::worker, this);
	} catch (std::exception &e) {
		LD("Exception in ctor: " + e.what());
	} catch (...) {
		LD("Exception in ctor - unknown");
	}
	LD("End ctor")
}


BluetoothInterface::~BluetoothInterface() {
	LD("Beg dtor")
	btint = nullptr;
	m_running = false;


	try {
		// Завершение процесса обработки
		if (m_thread.joinable()) {
			m_thread.join();
		}

		// Завершение цикла gLib
		if (m_loop) {
			g_main_loop_quit(m_loop);
			while (g_main_loop_is_running (m_loop)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}

		if (m_glib_thread.joinable()) {
			m_glib_thread.join();
		}

		// Освобождение контекста DBUS
		if (m_dbus_conn) {
			g_object_unref(m_dbus_conn);
		}

		if (m_adapter_proxy) {
			g_object_unref(m_adapter_proxy);
		}
	} catch (std::exception &e) {
		LD("Exception in dtor: " + e.what());
	} catch (...) {
		LD("Exception in dtor - unknown");
	}
	LI("Bluetooth interface stopped");
	LD("Dtor");
}



void BluetoothInterface::glib_worker() {
	LD("before glib_worker");
	try {
		g_main_loop_run (m_loop);
	} catch (std::exception &e) {
		LD("Exception in glib_worker: " + e.what());
	} catch (...) {
		LD("Exception in glib_worker")
	}
	LD("after glib_worker");
}

void BluetoothInterface::onSignal(GDBusConnection* connection, const gchar* sender_name, const gchar* object_path,
		const gchar* interface_name, const gchar* signal_name, GVariant* parameters, gpointer user_data)
{
	LD("Signal beg");
	try {
		LD("Signal:"  + signal_name + "  sender:" + sender_name + "  obj:" + object_path + "  int:" + interface_name + "  params: " + g_variant_get_type_string(parameters));
		if (std::string(signal_name) == "InterfacesAdded") {
			onInterfaceAdded(parameters);
		} else if (std::string(signal_name) == "PropertiesChanged") {
			onPropertiesChanged(object_path, parameters);
		}
	} catch (std::exception &e) {
		LD("Exception in onSignal: " + e.what());
	} catch (...) {
		LD("Exception in onSignal");
	}
	LD("Signal end");

}



int BluetoothInterface::notify(std::string addr, std::string parameter) {
	GError *err = nullptr;
	LT("Before set notify");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	try {
		GDBusProxy *charbtnProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr, "org.bluez",
				(addr + parameter).c_str(), "org.bluez.GattCharacteristic1", nullptr, &err);
		if (!charbtnProxy) {
			LE("notify GattCharacteristic1 not available:" + err->message);
			g_clear_error(&err);
			return -1;
		}

		if (!g_dbus_proxy_call_sync(charbtnProxy, "StartNotify", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
			LE("notify Failed to StartNotify: " + err->message + "(" + std::to_string(err->code) + ")");
			g_clear_error(&err);
			g_object_unref(charbtnProxy);
			return -1;
		}
		g_object_unref(charbtnProxy);
		LT("After set notify");

	} catch (std::exception &e) {
		LD("Exception in notify: " + e.what());
		return -1;
	} catch (...) {
		LD("Exception in notify");
		return -1;
	}

	return 0;
}


void BluetoothInterface::worker() {
	LI("Bluetooth interface started");


	try {
		while (m_running && (m_loop == nullptr || !g_main_loop_is_running(m_loop))) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			LD("wait till not g_main_loop_is_running");
		}

		GError *err = nullptr;
		m_dbus_conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);
		int ret =  g_dbus_connection_signal_subscribe(m_dbus_conn,
											"org.bluez",
											 NULL,
											 NULL,
											 NULL,
											 NULL,
											 G_DBUS_SIGNAL_FLAGS_NONE,
											 dbusSignalHandler,
											 NULL,
											 NULL);
		if (ret < 0) {
			LE("Subscribe is failed");
			return;
		}

		m_adapter_path = "/org/bluez/hci0";

		while (true) {
			if (!m_running) {
				return;
			}
			LD("Before g_dbus_proxy_new_for_bus_sync");
			m_adapter_proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr,
				"org.bluez", m_adapter_path.c_str(), "org.bluez.Adapter1", nullptr, &err);
			LD("After g_dbus_proxy_new_for_bus_sync");

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			if (!m_adapter_proxy) {
				LD("Adapter not found. Try once more...");
				g_clear_error(&err);
			} else {
				LI("Bluetooth adapter is found.");
				break;
			}
		}

		if (!m_adapter_proxy) {
			LE("Adapter not available:" + err->message);
			g_clear_error(&err);
			return;
		}

		m_init_ok = true;
		stopScan();
		removeDevices();

	} catch (std::exception &e) {
		LD("Exception in worker: " + e.what());
	} catch (...) {
		LD("Exception in worker");
	}
	LI("Bluetooth interface stoped 1");
}


int BluetoothInterface::bleConnect(std::string addr) {
	LD("bleConnect - " + addr);

	if (!m_init_ok) {
		LD("bleConnect - ret !m_init_ok");
		return -1;
	}

	try {
		GError *err = nullptr;
		const char* objPath = addr.c_str();
		GDBusProxy *devProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr,
				"org.bluez", objPath, "org.bluez.Device1", nullptr, &err);

		if (!devProxy) {
			LE("Device not available:" + err->message);
			g_clear_error(&err);
			return -1;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if (!g_dbus_proxy_call_sync(devProxy, "Connect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
			LE("Failed to connect: " + err->message + "(" + std::to_string(err->code) + ")");
			g_clear_error(&err);
			g_object_unref(devProxy);
			return -1;
		}
		g_object_unref(devProxy);

	} catch (std::exception &e) {
		LD("Exception in bleConnect: " + e.what());
		return -1;
	} catch (...) {
		LD("Exception in bleConnect");
		return -1;
	}

	LD("bleConnect - ok");
	return 0;
}


void BluetoothInterface::disconnect(BleDeviceSP d) {
	LD("Disconnect " + d->path);

	if (!m_init_ok) {
		LD("disConnect - ret !m_init_ok");
		return;
	}

	try {
		GError *err = nullptr;
		const char* objPath = d->path.c_str();
		d->connected = false;
		GDBusProxy *devProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr,
				"org.bluez", objPath, "org.bluez.Device1", nullptr, &err);

		if (!devProxy) {
			LE("Device not available:" + err->message);
			g_clear_error(&err);
			return;
		}

		if (!g_dbus_proxy_call_sync(devProxy, "Disconnect", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
			LE("Failed to disconnect: " + err->message + "(" + std::to_string(err->code) + ")");
			g_clear_error(&err);
		}
		g_object_unref(devProxy);
		LD("disConnect - ok");

	} catch (std::exception &e) {
		LD("Exception in disconnect: " + e.what());
	} catch (...) {
		LD("Exception in disconnect");
	}

}


int BluetoothInterface::bleSend(std::string addr, std::string parameter, std::string value) {
//	LD("bleSend - " + addr + "   param:" + parameter + "   value:" + value);

	if (!m_init_ok) {
		return -1;
	}


	std::lock_guard<std::mutex> lock(m_mutex);
	GError *err = nullptr;
	GDBusProxy *charflProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr, "org.bluez", (addr + parameter).c_str(), "org.bluez.GattCharacteristic1", nullptr, &err);
	if (!charflProxy) {
		LE("GattCharacteristic1 not available:" + err->message);
		g_clear_error(&err);
		return -1;
	}

	GVariant *char_value = g_variant_new_from_data(G_VARIANT_TYPE ("ay"), value.c_str(), value.size(), TRUE, NULL, NULL);
	if (char_value == NULL){
	    LE("Converting value error");
		g_object_unref(charflProxy);
	    return -1;
	}

	GVariantBuilder builder;
	g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
	GVariant* write_value_argument = g_variant_new("(@aya{sv})", char_value, &builder);
	if (!g_dbus_proxy_call_sync(charflProxy, "WriteValue", write_value_argument, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
		LE("Failed to WriteValue: " + err->message + "(" + std::to_string(err->code) + ")");
		std::string error {err->message};
		g_clear_error(&err);
		g_object_unref(charflProxy);
//		if (error.find("GDBus.Error:org.bluez.Error.InProgress") == 0) {
//			return 0;
//		} else {
			return -1;
//		}
	}

	g_object_unref(charflProxy);
	return 0;
}

void BluetoothInterface::startScan(std::function<void (BleDeviceSP)> on_device_found) {
	m_on_device_found = on_device_found;
		LD("Start discovery");

	if (!m_init_ok) {
		LD("Start discovery ret !m_init_ok");
		return;
	}

	try {
		//TODO: Реализовать пассивное сканирование через HCI интерфейс https://github.com/IanHarvey/bluepy/issues/198
		removeDevices();
	//	LD("Start discovery");
		GError *err = nullptr;
		if (!g_dbus_proxy_call_sync(m_adapter_proxy, "StartDiscovery", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
			LE("Failed to StartDiscovery: " + err->message + "(" + std::to_string(err->code) + ")");
			g_clear_error(&err);
			return;
		}

		m_need_scan = false;
		LD("Start discovery ok");

	} catch (std::exception &e) {
		LD("Exception in startScan: " + e.what());
	} catch (...) {
		LD("Exception in startScan");
	}
}


void BluetoothInterface::stopScan() {
	LD("Stop discovery");
	if (!m_init_ok) {
		LD("Stop discovery ret !m_init_ok");
		return;
	}

	try {
		//	LD("Stop discovery");
		GError *err = nullptr;
		if (!g_dbus_proxy_call_sync(m_adapter_proxy, "StopDiscovery", nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err)) {
			LE("Failed to StopDiscovery: " + err->message + "(" + std::to_string(err->code) + ")");
			g_clear_error(&err);
			return;
		}
		LD("Stop discovery ok");

	} catch (std::exception &e) {
		LD("Exception in stopScan: " + e.what());
	} catch (...) {
		LD("Exception in stopScan");
	}

}

void BluetoothInterface::printTypeOfVariant(GVariant* v) {
	if (g_variant_is_of_type(v, G_VARIANT_TYPE_STRING)) {
		LD("G_VARIANT_TYPE_STRING");
	}
	if (g_variant_is_of_type(v, G_VARIANT_TYPE_VARIANT)) {
		LD("G_VARIANT_TYPE_VARIANT");
	}
	if (g_variant_is_of_type(v, G_VARIANT_TYPE_BYTESTRING)) {
		LD("G_VARIANT_TYPE_BYTESTRING");
	}
	if (g_variant_is_of_type(v, G_VARIANT_TYPE_BYTESTRING)) {
		LD("G_VARIANT_TYPE_BYTESTRING");
	}
}


std::shared_ptr<BleObject> BluetoothInterface::sendInfoRequest(std::string path) {
	std::shared_ptr<BleObject> ret;

	if (!m_init_ok) {
		return ret;
	}

	std::lock_guard<std::mutex> lock(m_mutex);
	GError *err = nullptr;
	GDBusProxy *charflProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr, "org.bluez", path.c_str(), "org.bluez.Device1", nullptr, &err);
	if (!charflProxy) {
		LE("org.bluez.Device1 not available:" + err->message);
		g_clear_error(&err);
		return ret;
	}

//	LD("sendInfoRequest path: " + path);
	std::string value {"org.bluez.Device1"};
	GVariant *char_value = g_variant_new_from_data(G_VARIANT_TYPE ("s"), value.c_str(), value.size(), TRUE, NULL, NULL);
	if (char_value == NULL){
	    LE("Converting value error");
		g_object_unref(charflProxy);
	    return ret;
	}

//	LD("converting ok");
//	GVariantBuilder builder;
//	g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
//	GVariant* write_value_argument = g_variant_new("(@aya{sv})", char_value, &builder);
	GVariant *res = g_dbus_proxy_call_sync(charflProxy, "org.freedesktop.DBus.Properties.GetAll", g_variant_new("(s)", "org.bluez.Device1"), G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err);
	if (!res) {
		LE("Failed to GetAll: " + err->message + "(" + std::to_string(err->code) + ")");
		std::string error {err->message};
		g_clear_error(&err);
		g_object_unref(charflProxy);
//		if (error.find("GDBus.Error:org.bluez.Error.InProgress") == 0) {
//			return 0;
//		} else {
			return ret;
//		}
	}

	g_object_unref(charflProxy);
	return parseDevice(res);
}

void BluetoothInterface::addDevice(BleDeviceSP device) {
	LD("addDevice start");
	for (const auto &d : m_devices) {
		if (d->path == device->path) {
			return;
		}
	}

	m_devices.push_back(device);
	LD("addDevice stop");
}

void BluetoothInterface::onInterfaceAdded(GVariant* parameters) {
	LD("onInterfaceAdded start");
	std::shared_ptr<BleObject> obj = parseIntrospectable(parameters);

	if (!obj) {
		LD("onInterfaceAdded ret !obj");
		return;
	}

	if (BleDeviceSP dev = std::dynamic_pointer_cast<BleDevice>(obj)) {
//		LD("Device found: " + dev->name);
//		addDevice(dev);
		if (m_on_device_found) {
			m_on_device_found(dev);
		}
		m_device_cache[dev->path] = dev;
	} else if (std::shared_ptr<BleService> serv = std::dynamic_pointer_cast<BleService>(obj)) {
//		LD("Service found: " + serv->uuid);
		for (auto &d : m_devices) {
			if (d->path == serv->device && !std::any_of(d->services.begin(), d->services.end(), [&serv](auto s) {return serv->path == s->path;})) {
				d->services.push_back(serv);
				break;
			}
		}

		m_device_cache[serv->device]->services.push_back(serv);
	} else if (std::shared_ptr<BleCharacteristic> ch = std::dynamic_pointer_cast<BleCharacteristic>(obj)) {
//		LD("Characteristic found: " + ch->uuid);
		for (auto &d : m_devices) {
			for (auto &s : d->services) {
				if (s->path == ch->service && !std::any_of(s->characteristics.begin(), s->characteristics.end(), [&ch](auto c) {return ch->path == c->path;})) {
					s->characteristics.push_back(ch);
					d->characteristics.push_back(ch);
					goto exit;
				}
			}
		}
		exit:;

		for (auto &d : m_device_cache) {
			for (auto &s : d.second->services) {
				if (s->path == ch->service && !std::any_of(s->characteristics.begin(), s->characteristics.end(), [&ch](auto c) {return ch->path == c->path;})) {
					s->characteristics.push_back(ch);
					d.second->characteristics.push_back(ch);
					goto exit2;
				}
			}
		}
		exit2:;
//		m_device_cache[serv->device]->services.push_back(serv);
	}
	LD("onInterfaceAdded stop");
}

BleDeviceList BluetoothInterface::getCachedDevices() {
	LD("getCachedDevices start");
	BleDeviceList ret;

	std::lock_guard<std::mutex> lock(m_mutex);
	GError *err = nullptr;
	GDBusProxy *charflProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr, "org.bluez", "/", "org.freedesktop.DBus.ObjectManager", nullptr, &err);
	if (!charflProxy) {
		LE("org.bluez.Device1 not available:" + err->message);
		g_clear_error(&err);
		return ret;
	}

//	LD("GetManagedObjects");
	GVariant *res = g_dbus_proxy_call_sync(charflProxy, "GetManagedObjects",nullptr, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err);
	if (!res) {
		LE("Failed to GetManagedObjects: " + err->message + "(" + std::to_string(err->code) + ")");
		std::string error {err->message};
		g_clear_error(&err);
		g_object_unref(charflProxy);
//		if (error.find("GDBus.Error:org.bluez.Error.InProgress") == 0) {
//			return 0;
//		} else {
			return ret;
//		}
	}

//	LD("Parameters: " + g_variant_get_type_string(res));

	g_autoptr(GVariantIter) iter1 = NULL;
	GVariant *device;
	g_variant_get(res, "(a{?*})", &iter1);
	while (g_variant_iter_loop (iter1, "@{?*}", &device)) {
		BleDeviceSP dev = parseDevice(device);
		if (dev) {
			ret.push_back(dev);
		}
	}

	g_object_unref(charflProxy);
	LD("getCachedDevices stop");
	return ret;
}

void BluetoothInterface::onPropertiesChanged(std::string path, GVariant* parameters) {
	std::string type {g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL)};

	LD("onPropertiesChanged start");
//	LD("Type: " + type);

	gboolean serv_resolved;
	if (g_variant_lookup(g_variant_get_child_value(parameters, 1), "ServicesResolved", "b", &serv_resolved)) {
//		LD("ServicesResolved: " + (serv_resolved ? "True" : "False"));
		auto dev = std::find_if(m_devices.begin(), m_devices.end(), [&path](auto d){return d->path == path;});
		if (dev != m_devices.end()) {
			(*dev)->connected = serv_resolved;
			if ((*dev)->connected) {
				if (m_on_connect) {
//					LD("dev set connected");
					m_on_connect(*dev);
				}
			} else {
				if (m_on_disconnect) {
					m_on_disconnect(*dev);
				}
			}
		}
	} else if (type == "org.bluez.GattCharacteristic1") {
//		LD("param type: " + g_variant_get_type_string(g_variant_get_child_value(parameters, 1)));
		GVariant *value = g_variant_lookup_value(g_variant_get_child_value(parameters, 1), "Value", G_VARIANT_TYPE_ARRAY);
		if (value) {
			gsize n_elements;
//			LD("Value type: " + g_variant_get_type_string(value));
			gconstpointer dt = g_variant_get_fixed_array(value, &n_elements, 1);
			const char* cdt = static_cast<const char*>(dt);

			std::vector<unsigned char> ret;
			for (unsigned i = 0; i < n_elements; ++i) {
				ret.push_back(cdt[i]);
			}
//			LD(Utils::bytes2Hex(ret.data(), n_elements));
			if (!ret.empty()) {
				for (const auto &d : m_devices) {
					auto c = std::find_if(d->characteristics.begin(), d->characteristics.end(), [&path](auto d){return d->path == path;});
					if (c != d->characteristics.end()) {
						if ((*c)->on_notify) {
							(*c)->on_notify(ret);
						}
						break;
					}
				}
			}
		}
	}
	LD("onPropertiesChanged stop");
}


int BluetoothInterface::connect(std::string address) {
	if (address.empty()) {
		address = m_chosen_device;
	}

	if (address.empty()) {
		return -1;
	}

	LD("Try to connect to " + address);

	return bleConnect(address);
}


BleDeviceSP BluetoothInterface::parseDevice(GVariant* dev_var) {
	LD("parseDevice start");
	BleDeviceSP ret;
	std::string param_type {g_variant_get_type_string(dev_var)};

	if (param_type != "(oa{sa{sv}})" && param_type != "{oa{sa{sv}}}") {
		LE("Unexpected parameter type for InterfaceAdded event: " + param_type);
		return ret;
	}

	std::string path {g_variant_get_string(g_variant_get_child_value(dev_var, 0), NULL)};
	GVariant *device = g_variant_lookup_value(g_variant_get_child_value(dev_var, 1), "org.bluez.Device1", G_VARIANT_TYPE_VARDICT);
	if (device) {
		gchar *addr;
		gchar *name;
		if (g_variant_lookup(device, "Address", "s", &addr) && g_variant_lookup(device, "Name", "s", &name)) {
			ret = std::make_shared<BleDevice>(path, std::string{addr}, std::string{name});
//			LD("Parsed: " + ret->path + "   " + ret->address + "    " + ret->name);
		}
	}

	LD("parseDevice stop");
	return ret;
}

void BluetoothInterface::removeDevices() {
//	std::lock_guard<std::mutex> lock(m_mutex);
	LD("removeDevices start");
	GError *err = nullptr;

	m_need_scan = true;

	try {
		for (auto &d : getCachedDevices()) {
	//		LD("Try ro remove device " +  d->path);
			GVariant *res = g_dbus_proxy_call_sync(m_adapter_proxy, "RemoveDevice", g_variant_new("(o)", d->path.c_str()), G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err);
			if (!res) {
				LE("Failed to RemoveDevice: " + err->message + "(" + std::to_string(err->code) + ")");
				std::string error {err->message};
				g_clear_error(&err);
				return;
			}
		}

		LD("End removeDevices");

	} catch (std::exception &e) {
		LD("Exception in removeDevices: " + e.what());
	} catch (...) {
		LD("Exception in removeDevices");
	}
}

std::vector<unsigned char> BluetoothInterface::readCharacteristic(std::shared_ptr<BleCharacteristic> ch) {
	LD("readCharacteristic start");
	std::vector<unsigned char> ret;
	std::lock_guard<std::mutex> lock(m_mutex);
	GError *err = nullptr;

	try {
		GDBusProxy *charflProxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM, G_DBUS_PROXY_FLAGS_NONE, nullptr, "org.bluez", ch->path.c_str(), "org.bluez.GattCharacteristic1", nullptr, &err);
		if (!charflProxy) {
			LE("GattCharacteristic1 not available:" + err->message);
			g_clear_error(&err);
			return ret;
		}

		/* Create new variant builder. */
		GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));

		/* Finish building variant, i.e. the variant does not contain any value. */
		GVariant *v1 = g_variant_builder_end(builder);

		g_variant_builder_unref(builder);

		GVariant *res = g_dbus_proxy_call_sync(charflProxy, "ReadValue", g_variant_new_tuple(&v1, 1), G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &err);
		if (!res) {
			LE("Failed to ReadValue: " + err->message + "(" + std::to_string(err->code) + ")");
			std::string error {err->message};
			g_clear_error(&err);
			return ret;
		}

	//	LD("Got value: " + g_variant_get_type_string(res));

		gsize n_elements;
		gconstpointer dt = g_variant_get_fixed_array(g_variant_get_child_value(res, 0), &n_elements, 1);
		const char* cdt = static_cast<const char*>(dt);

		for (unsigned i = 0; i < n_elements; ++i) {
			ret.push_back(cdt[i]);
	//		LD(Utils::bytes2Hex(ret.data(), n_elements));
		}


		g_object_unref(charflProxy);
		LD("readCharacteristic stop");

	} catch (std::exception &e) {
		LD("Exception in readCharacteristic: " + e.what());
	} catch (...) {
		LD("Exception in rreadCharacteristic");
	}

	return ret;
}

std::vector<unsigned char> BluetoothInterface::readCharacteristic(BleDeviceSP d, std::string ch) {
	for (auto const &c : d->characteristics) {
		if (c->uuid == ch) {
			return readCharacteristic(c);
		}
	}
	return std::vector<unsigned char>{};
}

void BluetoothInterface::startNotify(BleDeviceSP d, std::string uuid, std::function<void(std::vector<unsigned char>)> on_notify) {
	auto ch = std::find_if(d->characteristics.begin(), d->characteristics.end(), [&uuid](auto c){return c->uuid == uuid;});
	if (ch != d->characteristics.end()) {
		(*ch)->on_notify = on_notify;
		notify((*ch)->path, "");
	}
}

bool BluetoothInterface::needScan() {
	return m_need_scan;
}

std::shared_ptr<BleObject> BluetoothInterface::parseIntrospectable(GVariant* var) {
	LD("parseIntrospectable start");
	std::shared_ptr<BleObject> ret;
	std::string param_type {g_variant_get_type_string(var)};

	if (param_type != "(oa{sa{sv}})" && param_type != "{oa{sa{sv}}}") {
		LE("Unexpected parameter type for InterfaceAdded event: " + param_type);
		return nullptr;
	}

	std::string path {g_variant_get_string(g_variant_get_child_value(var, 0), NULL)};
	GVariant *device;
	if ((device = g_variant_lookup_value(g_variant_get_child_value(var, 1), "org.bluez.Device1", G_VARIANT_TYPE_VARDICT))) {
		gchar *addr;
		gchar *name;
		if (g_variant_lookup(device, "Address", "s", &addr)) {
			if (g_variant_lookup(device, "Name", "s", &name)) {
				ret = std::make_shared<BleDevice>(path, addr, name);
			}
		}
	} else if ((device = g_variant_lookup_value(g_variant_get_child_value(var, 1), "org.bluez.GattService1", G_VARIANT_TYPE_VARDICT))) {
//		LD("GattService1 " + g_variant_get_type_string(device));
		gchar *uuid;
		gchar *dev;
		if (g_variant_lookup(device, "UUID", "s", &uuid) && g_variant_lookup(device, "Device", "o", &dev)) {
			LD("GattService1 parsed: " + uuid + "    " +  dev);
			ret = std::make_shared<BleService>(path, uuid, dev);
		}
	} else if ((device = g_variant_lookup_value(g_variant_get_child_value(var, 1), "org.bluez.GattCharacteristic1", G_VARIANT_TYPE_VARDICT))) {
//		LD("GattCharacteristic1 " + g_variant_get_type_string(device));
		gchar *uuid;
		gchar *service;
		if (g_variant_lookup(device, "UUID", "s", &uuid) && g_variant_lookup(device, "Service", "o", &service)) {
			ret = std::make_shared<BleCharacteristic>(path, uuid, service);
		}
	}

	LD("parseIntrospectable stop");
//	LD("Parsed: " + ret->path);
	return ret;
}
