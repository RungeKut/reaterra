#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <string> 		// string
#include <fstream>  	// ofstream
#include <mutex> 		// mutex lock_guard
#include <typeinfo>		// typeid
#include <vector>		// vector

// Закомментировать для блокировки вывода на консоль
//#define CONSOLE_LOG

// Закомментировать для разрешения кэширования вывода лога в файл
#define FLUSH_LOG

//TODO: Сделать правильный name demangling (https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html).
// Макросы для вывода в лог для использования с классами приложения
#define L(type, level, msg) Logger::instance().log(std::string(type) + ": " + msg, level);
#define LE(msg) Logger::instance().log(std::string(typeid(this).name()).substr(2) + ": " + msg, Logger::LogLevel::Error);
#define LI(msg) Logger::instance().log(std::string(typeid(this).name()).substr(2) + ": " + msg, Logger::LogLevel::Info);
#define LT(msg) Logger::instance().log(std::string(typeid(this).name()).substr(2) + ": " + msg, Logger::LogLevel::Trace);
#define LD(msg) Logger::instance().log(std::string(typeid(this).name()).substr(2) + ": " + msg, Logger::LogLevel::Debug);
#define LG(msg) Logger::instance().log(std::string("Global: ") + msg, Logger::LogLevel::Debug);

#define LEt(type, msg) Logger::instance().log(type + ": " + msg, Logger::LogLevel::Error);
#define LIt(type, msg) Logger::instance().log(type + ": " + msg, Logger::LogLevel::Info);
#define LTt(type, msg) Logger::instance().log(type + ": " + msg, Logger::LogLevel::Trace);
#define LDt(type, msg) Logger::instance().log(type + ": " + msg, Logger::LogLevel::Debug);

/// Работа с лог файлом.
class Logger {

public:
	/// Уровни сообщений
	enum class LogLevel {
		Error,	///< Информация об ошибках.
		Info,	///< Общая информация.
		Trace,  ///< Расширенная информация.
		Debug   ///< Отладочная информация.
	};

	/*!
	 * @brief Получение объекта.
	 *
	 * @param app_name название приложения.
	 * @param level уровень сообщений.
	 * @param console_flag флаг вывода на консоль.
	 * @return объект класса.
	 */
	static Logger& instance(std::string app_name = "Application", LogLevel level = LogLevel::Info, bool console_flag = true);


	/*!
	 * @brief Основная функция для вывода в лог.
	 *
	 * @param msg текст сообщения.
	 * @param level уровень информации в сообщении.
	 */
    void log(std::string msg, LogLevel level = LogLevel::Info);

    /*!
     * @brief Установка уровня сообщений.
     *
     * @param level уровень сообщений.
     */
    void setLogLevel(std::string level);

    /// Сброс буферов на диск.
    void flush();

private:

    //TODO: Настойки логгера - уровень лога, вывод в консоль, размер лога и количество бэкапов хранить в файле с именем <имя приложения>_log.cfg. Если файл не найден то создать его с параметрамио умолчанию.
	explicit Logger(std::string app_name, LogLevel level, bool console_flag);
	virtual ~Logger();
	static const long LOG_FILE_SIZE_LIMIT {10000000};
	static const long NUM_OF_LOG_FILES_LIMIT {10};
	static const std::vector<std::string> level_names;

    LogLevel m_log_level;
	long m_log_lenght;
	std::ofstream m_log_stream;
	std::mutex m_mutex;
	std::string m_app_name;
	std::string m_log_file_name;
	bool m_console_flag;

	void log_rotate();

};
#endif /* SRC_LOGGER_H_ */
