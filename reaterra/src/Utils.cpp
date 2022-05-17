#include <cmath>
#include <cxxabi.h>
#include <fcntl.h>			// fcntl, open O_RDWR O_NOCTTY
#include <string.h> 		// strdup, strchr
#include <sys/stat.h> 		// stat, mkdir, S_ISDIR
#include <termios.h>		// tcgetattr B9600 cfsetospeed
#include <unistd.h>			// unlink close

#include <algorithm>
#include <chrono>
#include <iomanip>			// setw setfill
#include <iostream> 		// cerr
#include <memory> 			// shared_ptr
#include <sstream>
#include <codecvt>

#include "Logger.h"
#include "Utils.h"

namespace Utils {

	//Create folder if not existed. Returns 0: success / -1: fail or file with the same name existed
	int makeDir(const char *path) {
		struct stat st;
		int ret = 0;

		if (stat(path, &st) != 0) {
			if (mkdir(path, 0700) != 0) {
				ret = -1;
			}
		} else if (!S_ISDIR(st.st_mode)) {
			ret = -1;
		}

		return ret;
	}

	//Create path if not existed.  Returns 0: success / -1: fail
	int makePath(const char *path) {
		char *pp;
		char *sp;
		int ret;
		std::shared_ptr<char> copypath = std::shared_ptr<char>(strdup(path));

		ret = 0;
		pp = copypath.get();
		while (ret == 0 && (sp = strchr(pp, '/')) != 0) {
			if (sp != pp) {
				*sp = '\0';
				ret = makeDir(copypath.get());
				*sp = '/';
			}
			pp = sp + 1;
		}
		if (ret == 0) {
			ret = makeDir(path);
		}

		return ret;
	}

namespace {
	std::string lock_file_name;
}
	void unlockFile(void) {
		if (!lock_file_name.empty()) {
			unlink(lock_file_name.c_str());
		}
	}


	bool lockFile(std::string lock_file) {
		if (lock_file.empty()) {
			std::cerr << "Error! Lock file name is empty" << std::endl;
			return false;
		}
		lock_file_name = lock_file + "_lock";
		int fd;
		struct flock fl;

		fd = open(lock_file_name.c_str(), O_RDWR | O_CREAT, 0600);
		if (fd < 0) {
			std::cerr << "Error! Couldn't open " << lock_file_name << " file" << std::endl;
			return false;
		}

		fl.l_start = 0;
		fl.l_len = 0;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		if (fcntl(fd, F_SETLK, &fl) < 0) {
			return false;
		}

		atexit(unlockFile);
		return true;
	}


	int openSerial(std::string port_name) {
		int fd ;

		fd = open(("/dev/" + port_name).c_str(), O_RDWR | O_NOCTTY);
		if (fd < 0) {
			return fd;
		}

	    struct termios options;

	    tcgetattr(fd, &options);
	    cfsetispeed(&options, B115200);
	    cfsetospeed(&options, B115200);

	    //TODO: Check TTY options flags
	    //Setting TTY flags
	    options.c_iflag = 0;		//Reset options iflag
	    //IGNBRK; // Ignore BREAK condition on input.
	    options.c_iflag &= ~IXON; 	// Enable XON/XOFF flow control on output.
	    options.c_iflag &= ~IXOFF; 	// Enable XON/XOFF flow control on input.
	    options.c_cflag |= CS8; 	// Р§РёСЃР»Рѕ Р±РёС‚ РїРѕРґ 1 Р±Р°Р№С‚
	    options.c_cflag &= ~PARENB; // РќРµ РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Р±РёС‚ С‡РµС‚РЅРѕСЃС‚Рё
	    options.c_cflag &= ~CSTOPB; // Р�СЃРїРѕР»СЊР·РѕРІР°С‚СЊ 1 СЃС‚РѕРї-Р±РёС‚
	    options.c_cflag |= CREAD; 	// Enable Receiver
	    options.c_cflag |= CLOCAL; 	// Ignore modem control lines.
//	    options.c_cflag |= CRTSCTS;// Enable hardware handshake.
	    options.c_cflag &= ~CRTSCTS;// Disable hardware handshake.
	    options.c_lflag &= ~ISIG; 	// РќРµ РіРµРЅРµСЂРёСЂРѕРІР°С‚СЊ СЃРёРіРЅР°Р»С‹ РїСЂРё РїРѕР»СѓС‡РµРЅРёРё РґР°РЅРЅС‹С…
	    options.c_lflag &= ~ICANON; // Р—Р°РїСЂРµС‚РёС‚СЊ EOF Рё РїСЂРѕС‡РёРµ СЃРїРµС† СЃРёРјРІРѕР»С‹
	    options.c_lflag &= ~XCASE; 	// (not in POSIX; not supported under Linux) If ICANON is also set, terminal is uppercase only. Input is converted to lowercase, except for characters preceded by \. On output, uppercase characters are preceded by \ and lowercase characters are converted to uppercase.
	    options.c_lflag &= ~ECHO; 	// Echo input characters.
	    options.c_oflag &= ~OPOST;
	    options.c_cc[VTIME] = 10; 	// СЃРєРѕР»СЊРєРѕ Р¶РґРµРј РґР°РЅРЅС‹Рµ РґРµСЃСЏС‚РєРё РґРѕР»Рё СЃРµРє
	    options.c_cc[VMIN] = 0; 	// РїРѕР·РІРѕР»СЏРµРј С‡С‚РµРЅРёСЋ РІС‹С…РѕРґРёС‚СЊ РґР°Р¶Рµ РµСЃР»Рё РґР°РЅРЅС‹С… РЅРµС‚

	    //Р—Р°РїРёСЃС‹РІР°РµРј РїР°СЂР°РјРµС‚СЂС‹ РІ СѓСЃС‚СЂРѕР№СЃС‚РІРѕ
	    tcflush(fd, TCIOFLUSH);
	    tcsetattr(fd, TCSAFLUSH, &options);

	    return fd;
	}


	std::string int2Hex(int n, int wide) {
		std::stringstream stream;
		if (wide != 0) {
			stream << std::setw(wide) << std::setfill('0');
		}
		stream << std::hex << n;
		return stream.str();
	}

	std::string bytes2Hex(const unsigned char* bytes, int size, bool delim) {
		std::string ret;
		for (int i = 0; i < size; ++i) {
			ret += int2Hex(bytes[i], 2);
			if (i < size - 1) {
				if (delim) {
					ret += " ";
				}
			}
		}

		return ret;
	}

	unsigned short ba2S(unsigned char low, unsigned char high) {
		return (high << 8) + low;
	}

	unsigned char s2LB(unsigned short s) {
		return s & 0xFF;
	}

	unsigned char s2HB(unsigned short s) {
		return s >> 8;
	}


	// convert UTF-8 string to wstring
	std::wstring utf8_to_wstring(const std::string& str) {
	    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	    return myconv.from_bytes(str);
	}

	// convert wstring to UTF-8 string
	std::string wstring_to_utf8(const std::wstring& str) {
	    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	    return myconv.to_bytes(str);
	}

	// convert UTF-8 string to u16string
	std::u16string utf8_to_u16string(const std::string& str) {
	    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;
	    return myconv.from_bytes(str);
	}

	// convert u16string to UTF-8 string
	std::string u16string_to_utf8(const std::u16string& str) {
	    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> myconv;
	    return myconv.to_bytes(str);
	}
	std::string getFileTimestamp() {
		char time_buff[64];
		time_t t {time(nullptr)};
		struct tm *timeinfo = localtime(&t);
		strftime(time_buff, sizeof(time_buff), "%F.%H%M%S", timeinfo);
		return std::string(time_buff);
	}

	std::string getCurrentTimeString(std::string format) {
		char time_buff[64];
		strftime(time_buff, sizeof(time_buff), format.c_str(), getCurrentTime());
		return std::string(time_buff);
	}

	struct tm *getCurrentTime() {
		time_t t {time(nullptr)};
		return localtime(&t);
	}


	std::string getLogTimestamp() {
		std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());

		std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
		std::time_t t = static_cast<std::time_t>(s.count());
		std::size_t fract = ms.count() % 1000;


		char buffer[80];
		struct tm *timeinfo = localtime(&t);
		strftime(buffer, sizeof(buffer), "%F %T", timeinfo);
		std::stringstream ss;
		ss << buffer << "." << std::setw(3) << std::setfill('0') << fract;

		return ss.str();
	}


	float deg2Radian(float deg) {
		return deg * M_PI / 180.0;
	}


	float ms2kmh(float ms) {
		return ms * 18. / 5.;
	}

	float milih2kmh(float milih) {
		return milih / 0.62137119223733;
	}

	float kmh2ms(float kmh) {
		return kmh * 5. / 18.;
	}

	float kmh2milih(float kmh) {
		return kmh * 0.62137119223733;
	}

	std::string allTrim(std::string str) {
		if (str.empty()) {
			return str;
		}
	    size_t first = str.find_first_not_of(' ');
	    if (std::string::npos == first)
	    {
	        return "";
	    }
	    size_t last = str.find_last_not_of(' ');
	    return str.substr(first, (last - first + 1));
	}

	float physConvert(Phys phys, Unit from, Unit to, float value) {
		float ret = -1;

		if (from == to) {
			return value;
		}

		switch (phys) {
		case Phys::Angle:
			if (from == Unit::Degree && to == Unit::Radian) {
				return deg2Radian(value);
			} else if (from == Unit::Radian && to == Unit::Degree) {
				return value * 180.0 / M_PI;
			} else if (from == Unit::Degree && to == Unit::Procent) {
				return tan(physConvert(Phys::Angle, Unit::Degree, Unit::Radian, value)) * 100.;
			} else if (from == Unit::Procent && to == Unit::Degree) {
				return physConvert(Phys::Angle, Unit::Radian, Unit::Degree, atan(value / 100));
			} else {
				throw std::runtime_error("Unsupported angle conversion");
			}
			break;
		case Phys::Distance:
			if (from == Unit::Meter && to == Unit::KiloMeter) {
				return value / 1000.;
			} else if (from == Unit::Meter && to == Unit::Mile) {
				return value * 0.00062137119223733;
			} else if (from == Unit::KiloMeter && to == Unit::Mile) {
				return value * 0.62137119223733;
			} else if (from == Unit::KiloMeter && to == Unit::Meter) {
				return value * 1000;
			} else if (from == Unit::Mile && to == Unit::Meter) {
				return value * 1609.34;
			} else {
				throw std::runtime_error("Unsupported distance conversion");
			}
			break;
		case Phys::Energy:
			if (from == Unit::Joul && to == Unit::KiloKall) {
				return value / 4184.0;
			} else if (from == Unit::Joul && to == Unit::KWattPerHour) {
				return value / 3600000.0;
			} else if (from == Unit::KWattPerHour && to == Unit::Joul) {
				return value * 3600000.0;
			} else {
				throw std::runtime_error("Unsupported energy conversion");
			}
			break;
		case Phys::Power:
			if (from == Unit::Watt && to == Unit::KiloWatt) {
				return value / 1000.;
			} else if (from == Unit::KiloWatt && to == Unit::Watt) {
				return value * 1000;
			} else {
				throw std::runtime_error("Unsupported power conversion");
			}
			break;
		case Phys::PowerAir:
			if (from == Unit::MlMinKg && to == Unit::MET) {
				return value / 3.5;
			} else {
				throw std::runtime_error("Unsupported power air conversion");
			}
			break;
		case Phys::Speed:
			if (from == Unit::KmPerHour && to == Unit::MPerSec) {
				return value * 5 / 18.;
			} else if (from == Unit::KmPerHour && to == Unit::MilePerHour) {
				return value * 0.62137119223733;
			} else if (from == Unit::MilePerHour && to == Unit::KmPerHour) {
				return value / 0.62137119223733;
			} else if (from == Unit::MPerSec && to == Unit::KmPerHour) {
				return value / 5 * 18;
			} else {
				throw std::runtime_error("Unsupported speed conversion");
			}
			break;
		}

		return ret;
	}

	std::string formatFloat(std::string fmt, float value) {
		char buff[32];
		sprintf(buff, fmt.c_str(), value);
		return std::string(allTrim(buff));
	}

	std::string formatTime(int sec) {
		char buff[9];
		sprintf(buff, "%02d:%02d:%02d", sec / 60 / 60 % 60, sec / 60 % 60, sec % 60);
		return std::string(buff);
	}

	bool pathExist(std::string path) {
		struct stat st;
		if (stat(path.c_str(), &st) != 0) {
			return true;
		}
		return false;
	}

	int setDateTime(const struct tm *time) {

		char buffer[64];
		strftime(buffer, sizeof(buffer), "%F %T", time);
		LG("Time set: " + std::string("sudo date --set '") + buffer + "'");

		if (int ret = system((std::string("sudo date --set '") + buffer + "'").c_str())) {
			return ret;
		}

		LG("Time set OK");
		if (int ret = system("sudo hwclock -w")) {
			return ret;
		}
		LG("Time write OK: sudo hwclock -w");

		return 0;
	}

	bool isLeapYear(unsigned year) {
		return (!(year % 4) && year % 100) || !(year % 400) ? true : false;
	}

	std::string typeName(const std::type_info& type_inf) {
		int res;
		char *n = abi::__cxa_demangle(type_inf.name(), 0, 0, &res);
		std::string name {n};
		free(n);
		return name;
	}

	std::vector<std::string> split(std::string str, char delimiter) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(str);
		while (std::getline(tokenStream, token, delimiter))
		{
		  tokens.push_back(token);
		}
		return tokens;
	}

	std::string unsplit(std::vector<std::string> vec, char delimiter) {
		if (vec.size() < 1) {
			return std::string();
		}

		std::stringstream ss;
		for (int i = 0; i < static_cast<int>(vec.size()) - 1; ++i) {
			ss << vec.at(i) << delimiter;
		}
		ss << vec.back();
		return ss.str();
	}

	std::string getFio(const std::string last_name, const std::string first_name, const std::string middle_name) {
		setlocale(LC_ALL, "");
		std::wstring first = first_name.empty() ? L"" : utf8_to_wstring(first_name).substr(0, 1) + L".";
		std::wstring middle = middle_name.empty() ? L"" : utf8_to_wstring(middle_name).substr(0, 1) + L".";
		std::wstring str = utf8_to_wstring(last_name) + (first.empty() ? L"" : L" " + first + middle);
		std::transform(str.begin(), str.end(), str.begin(), ::towupper);

		return wstring_to_utf8(str);
	}

}

