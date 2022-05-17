#include "Context.h"

std::string Context::main_board {"PI"};

Context::Context()
: m_settings ("reaterra.cfg", {
		{"treadmill_type", "0"},
		{"admin_pwd", "357"},
		{"modbus_debug", "0"},
		{"modbus_send_delay", "0"},
		{"modbus_addr_shift", "0"},
		{"modbus_queue_limit", "20"},
		{"modbus_attempts", "3"},
		{"treadmill_poll_time", "500"},
		{"treadmill_modbus_address", "1"},
		{"treadmill_port_name", "/dev/ttyAMA0"},
		{"treadmill_port_speed", "38400"},
		{"angle_min", "0"},
		{"angle_max", "1400"},
		{"speed_min", "1"},
		{"sounds_volume", "100"},
		{"locale", "RU"},
		{"check_connection", "1"},
		{"Формат_Мощность_кВт", "%4.3f"},
		{"Формат_Мощность_Вт", "%3.1f"},
		{"Формат_МощностьАэробная_МЕТ", "%4.2f"},
		{"Формат_МощностьАэробная_мл/мин/кг", "%3.1f"},
		{"Формат_Расстояние_мили", "%5.3f"},
		{"Формат_Расстояние_км", "%5.3f"},
		{"Формат_Расстояние_м", "%5.0f"},
		{"Формат_Энергия_Дж", "%6.0f"},
		{"Формат_Энергия_ккал", "%5.3f"},
		{"Формат_Энергия_кВт·ч", "%6.4f"},
		{"Формат_Угол_град", "%4.1f"},
		{"Формат_Угол_%", "%4.1f"},
		{"Формат_Скорость_км/ч", "%3.1f"},
		{"Формат_Скорость_м/с", "%3.1f"},
		{"Формат_Скорость_миля/ч", "%3.1f"},
		{"Формат_Ускорение_м/с2", "%5.3f"},
		{"log_db_requests", "0"},
		{"power_off_pin", "20"},
		{"display_pwm_freq_Hz", "100"},
		{"display_pwn_duty_%", "20"},
		{"sas_enable", "0"},
		{"test_mode", "0"},
		{"controller_version", "1"},
		{"titabit_port", "5200"},
		{"touch_path", "/dev/input/event0"}
	}),

	//TODO: Стоит ли привязывать сохранение к пациенту?
	m_patient {""},
	m_patient_id {-1},
	m_profile_id {0},
	m_patient_weight {0.},
	m_first_run {true}
{
	m_accel = std::stoi(m_db.getParam("УровеньУскоренияДорожки", "4"));
	m_decel = std::stoi(m_db.getParam("УровеньТорможенияДорожки", "4"));
	m_notuse_doc_password = m_db.getParam("ОтключитьПарольВрача", "0") == "1" ? true : false;
}

Context::~Context() {
}

