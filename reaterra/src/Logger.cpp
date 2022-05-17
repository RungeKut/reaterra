#include "Logger.h"

#include <dirent.h>		// DIR dirent opendir readdir closedir
#include <string.h> 	// strcpy strcat
#include <sys/stat.h>   // stat

#include <algorithm>	// sort
#include <chrono>
#include <iomanip> 		// setw setfill
#include <iostream> 	// cout
#include <sstream> 		// stringstream

#include "Context.h"
#include "Utils.h"
#include "Version.h"


const std::vector<std::string> Logger::level_names {
	"E", "I", "T", "D"
};

//TODO: придумать как сделать включение выкл логов в консоль - может по наличию файла с флагом внутри
Logger::Logger(std::string app_name, LogLevel level, bool console_flag)
:	m_app_name {app_name},
	m_console_flag {console_flag}
{

	if (m_app_name.empty()) {
		std::cerr << "Error! Application log name is empty." << std::endl;
		exit(1);
	}

	std::string app {m_app_name};
	std::transform(app.begin(), app.end(), app.begin(), [](char ch) {
		ch = ::tolower(ch);
	    return ch == ' ' ? '_' : ch;
	});

	if (!Utils::lockFile(app)) {
		std::cerr << "Error! Program is already running." << std::endl;
		exit(1);
	}


	m_log_file_name = app + "_log.txt";

	struct stat stat_buff;
	if (stat(m_log_file_name.c_str(), &stat_buff) == 0) {
		m_log_lenght = stat_buff.st_size;


		struct dirent *ent;
		std::vector<std::string> file_list;

		DIR *dir = opendir(".");
		if (dir != NULL) {
			while ((ent = readdir(dir)) != NULL) {
				std::string entry {ent->d_name};
				if (entry.find(m_log_file_name) == std::string::npos || entry == m_log_file_name) {
					continue;
				}
				file_list.push_back(entry);
			}
			closedir(dir);
		} else {
			//TODO add fail processing
		}

		std::sort(file_list.begin(), file_list.end());
		if (file_list.size() > NUM_OF_LOG_FILES_LIMIT) {
			for (std::size_t i = 0; i < file_list.size() - NUM_OF_LOG_FILES_LIMIT; ++i) {
				remove(file_list.at(i).c_str());
			}
		}
	}
	m_log_stream.open(m_log_file_name, std::ios_base::out | std::ios_base::app);

	char buff[10];
	sprintf(buff, "%d.%d", VERSION / 100, VERSION % 100);
	m_log_level = level;
	log("------ " + m_app_name + " started ------");
	log(std::string{"- Version: "} + buff + "  Build date[" + __DATE__ + "] time[" + __TIME__ + "] -");

	std::ifstream model_file;
	std::string model_str;
	model_file.open("/proc/device-tree/model");
    std::getline(model_file, model_str);
    if (model_str.find("Compute Module") != model_str.npos) {
    	Context::main_board = "CM";
    }

    log("Main board model: " + model_str + " [will be used " + Context::main_board + " profile]");
}

Logger::~Logger() {
	log("------ " + m_app_name + " stopped ------");
}

void Logger::log(std::string msg, LogLevel level) {

	if (level > m_log_level) {
		return;
	}

	if (std::isdigit(msg[0])) {
		msg.erase(0, 1);
	}

	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());

	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
	std::time_t t = static_cast<std::time_t>(s.count());
	std::size_t fract = ms.count() % 1000;


	char buffer[80];
	struct tm *timeinfo = localtime(&t);
	strftime(buffer, sizeof(buffer), "%F %T", timeinfo);
	std::stringstream ss;
	ss << buffer << "." << std::setw(3) << std::setfill('0') << fract << ":" << level_names.at(static_cast<int>(level)) << ": " << msg << "\n";

	try {
		std::lock_guard<std::mutex> lock{m_mutex};
		m_log_lenght += ss.str().size();

		if (m_log_lenght > LOG_FILE_SIZE_LIMIT) {
			log_rotate();
			m_log_lenght = 0;
		}

//#ifdef CONSOLE_LOG
		if (m_console_flag) {
			std::cout << ss.str();
			std::cout.flush();
		}
//#endif

		m_log_stream << ss.str();

#ifdef FLUSH_LOG
		m_log_stream.flush();
#endif

	}  catch (const std::exception& ex) {
		//TODO process fail cases
		throw ex;
	}
}


void Logger::log_rotate() {
	m_log_stream.close();

	char name_buff[128];
	strcpy(name_buff, m_log_file_name.c_str());

	char time_buff[64];
	time_t t {time(nullptr)};
	struct tm *timeinfo = localtime(&t);
	strftime(time_buff, sizeof(time_buff), ".%F.%H%M", timeinfo);
	strcat(name_buff, time_buff);
	if(rename(m_log_file_name.c_str(), name_buff) == -1) {
	    std::cerr << "Error: Renaming log file - " << strerror(errno) << std::endl;
	}

	m_log_stream.open(m_log_file_name, std::ios_base::out | std::ios_base::app);
}


Logger& Logger::instance(std::string app_name, LogLevel level, bool console_flag) {
	static Logger instance (app_name, level, console_flag);
	return instance;
}

void Logger::setLogLevel(std::string level) {
	auto li = find(level_names.begin(), level_names.end(), level);
	if (li != level_names.end()) {
		m_log_level = static_cast<LogLevel>(li - level_names.begin());
	}
}

void Logger::flush() {
	m_log_stream.flush();
}
