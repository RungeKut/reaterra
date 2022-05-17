#include "DisplayLightnessPwm.h"

#include "Logger.h"
#include "Context.h"
#include "GpioDriver.h"

DisplayLightnessPwm::DisplayLightnessPwm() {
	LD("Ctor");
	int freq = ctx.m_settings.getParamInt("display_pwm_freq_Hz");
	if (freq < 10) {
		freq = 10;
	} else if (freq > 5000) {
		freq = 5000;
	}
	m_period_micros = 1000000 / freq;

	m_duty = ctx.m_settings.getParamInt("display_pwn_duty_%");
	if (m_duty < 1) {
		m_duty = 1;
	} else if (m_duty > 99) {
		m_duty = 100;
	}
	m_up_time_micros = m_period_micros * m_duty / 100;
	m_fd_sysfs = -1;




	LD("m_up_time_micros:" + std::to_string(m_up_time_micros) + "   down:" + std::to_string(m_period_micros - m_up_time_micros));
	start();
}

DisplayLightnessPwm::~DisplayLightnessPwm() {
	close(m_fd_sysfs);
	LD("Dtor");
}

bool DisplayLightnessPwm::process() {
	if (m_duty == 100) {
		return false;
	}
//	LD("tick");
	std::this_thread::sleep_for(std::chrono::microseconds(m_up_time_micros));
	if (write(m_fd_sysfs, "0", 2) < 0) {
//		close(m_fd_sysfs);
//		throw std::runtime_error("Failed to write value to gpio!");
	}
//	LD("tock");
//	GpioDriver::setPinLevel(44, GpioDriver::PinLevel::Low);
	std::this_thread::sleep_for(std::chrono::microseconds(m_period_micros - m_up_time_micros));
	if (write(m_fd_sysfs, "1", 2) < 0) {
//		close(m_fd_sysfs);
//		throw std::runtime_error("Failed to write value to gpio!");
	}
//	GpioDriver::setPinLevel(44, GpioDriver::PinLevel::High);

	return true;
}

bool DisplayLightnessPwm::init() {
	GpioDriver::setPinDirection(45, GpioDriver::PinDirection::Out);
	GpioDriver::setPinLevel(45, GpioDriver::PinLevel::High);
	std::string pin_str { std::to_string(45) };
	std::string pin_path { "/sys/class/gpio/gpio" + pin_str + "/value" };
	m_fd_sysfs = open(pin_path.c_str(), O_WRONLY);
	if (m_fd_sysfs < 0) {
		throw std::runtime_error("Failed to open gpio value for writing!");
	}
	return true;
}
