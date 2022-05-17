#include "GpioDriver.h"

#include <fcntl.h>
#include <unistd.h>

#include "Logger.h"

namespace GpioDriver {

void setPinDirection(int pin, PinDirection dir) {
	int fd;
//    	LDt(std::string("GpioDriver"), "Try to export GPIO.");
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		throw std::runtime_error("Failed to open export!");
	}
	std::string pin_str { std::to_string(pin) };
	write(fd, pin_str.c_str(), pin_str.size() + 1);
	close(fd);
	usleep(100000);
	//set GPIO direction code
	std::string dir_path { "/sys/class/gpio/gpio" + pin_str + "/direction" };
	fd = open(dir_path.c_str(), O_WRONLY);
	if (fd < 0) {
		throw std::runtime_error("Failed to open gpio direction for writing!");
	}
	std::string dir_str { dir == PinDirection::In ? "in" : "out" };
	if (write(fd, dir_str.c_str(), dir_str.size() + 1) < 0) {
		throw std::runtime_error("Failed to set direction!");
	}
	close(fd);
//    	LDt(std::string("GpioDriver"), "RPI pin exported and pin direction configured successfully.");
}

void setPinLevel(int pin, PinLevel level) {
	int fd;
	std::string pin_str { std::to_string(pin) };
	std::string pin_path { "/sys/class/gpio/gpio" + pin_str + "/value" };
	fd = open(pin_path.c_str(), O_WRONLY);
	if (fd < 0) {
		throw std::runtime_error("Failed to open gpio value for writing!");
	}

	std::string pin_value { level == PinLevel::Low ? "0" : "1" };
	if (write(fd, pin_value.c_str(), pin_value.size() + 1) < 0) {
		close(fd);
		throw std::runtime_error("Failed to write value to gpio!");
	}
//		LDt(std::string("GpioDriver"), "GPIO" + pin_str + " written : " + pin_value + " successfully !");
	close(fd);
}

PinLevel getPinLevel(int pin) {
	int fd;
	char pin_value[3];
	std::string pin_str { std::to_string(pin) };
	std::string pin_path { "/sys/class/gpio/gpio" + pin_str + "/value" };
	fd = open(pin_path.c_str(), O_RDONLY);
	if (fd < 0) {
		throw std::runtime_error("Failed to open gpio value for reading!");
	}

	if (read(fd, pin_value, 3) < 1) {
		close(fd);
		throw std::runtime_error("Failed to write value to gpio!");
	}

	pin_value[1] = 0;
//		LDt(std::string("GpioDriver"), "GPIO" + pin_str + " read : " + pin_value + " successfully !");
	close(fd);

	return atoi(pin_value) ? PinLevel::High : PinLevel::Low;
}

}
