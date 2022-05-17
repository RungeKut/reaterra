#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <string>

/// Вспомогательные утилиты.
namespace Utils {

	/// Физические величины.
	enum class Phys {
		Speed,			///< Скорость.
		Distance,		///< Расстояние.
		Energy,			///< Энергия.
		Power,			///< Механическая мощность.
		PowerAir,		///< Аэробная мощность.
		Angle			///< Угол наклона.
	};

	/// Единицы измерения.
	enum class Unit {
		Meter,			///< Метры.
		KiloMeter,		///< Километры.
		Mile,			///< Мили.
		KmPerHour,		///< Километры в час
		MPerSec,		///< Метры в секунду.
		MilePerHour,	///< Мили в час.
		Degree,			///< Градусы.
		Radian,			///< Радианы.
		Procent,		///< Проценты.
		KiloKall,		///< Килокалории.
		Joul,			///< Джоули.
		KWattPerHour,	///< Киловатты в час.
		Watt,			///< Ватты.
		KiloWatt,		///< Киловатты.
		MET,			///< МЕТ-ы
		MlMinKg,		///< мл/мин/кг.
		Unknown			///< Неизвестный.
	};

	/*!
	 * @brief Создать каталог.
	 *
	 * @param path имя каталога.
	 * @return 0 - успешно.
	 */
	int makeDir(const char *path);

	/*!
	 * @brief Создать путь (с подкаталогами).
	 *
	 * @param path путь.
	 * @return 0 - успешно.
	 */
	int makePath(const char *path);

	/*!
	 * @brief Занять лок файл.
	 *
	 * @param lock_file имя файла.
	 * @return true: успех.
	 */
	bool lockFile(std::string lock_file);

	/*!
	 * @brief Открыть последовательный порт.
	 *
	 * @param port_name имя порта.
	 * @return -1: ошибка.
	 */
	int openSerial(std::string port_name);

	/*!
	 * @brief Преобразование числа в шестнадцатеричную строку.
	 *
	 * @param n число.
	 * @param wide ширина результата.
	 * @return шестнадцатеричная строка.
	 */
	std::string int2Hex(int n, int wide = 0);

	/*!
	 * @brief Преобразование массива байт в шестнадцатеричную строку.
	 *
	 * @param bytes массив байт.
	 * @param size размер массива.
	 * @param delim использовать или нет разделитель.
	 * @return шестнадцатеричная строка.
	 */
	std::string bytes2Hex(const unsigned char *bytes, int size, bool delim = true);

	/*!
	 * @brief Преобразование двух байт в unsigned short.
	 *
	 * @param low младший байт.
	 * @param high старший байт.
	 * @return результат преобразования.
	 */
	inline unsigned short ba2S(unsigned char low, unsigned char high);

	/*!
	 * @brief Выделение младшего байта из unsigned short.
	 *
	 * @param s значение unsigned short.
	 * @return младший байт.
	 */
	inline unsigned char s2LB(unsigned short s);

	/*!
	 * @brief Выделение старшего байта из unsigned short.
	 *
	 * @param s значение unsigned short.
	 * @return старший байт.
	 */
	inline unsigned char s2HB(unsigned short s);

	/*!
	 * @brief Преобразование строки в кодировке UTF-8 в строку wstring.
	 *
	 * @param str строка в кодировке UTF-8.
	 * @return строка wstring.
	 */
	std::wstring utf8_to_wstring(const std::string& str);

	/*!
	 * @brief Преобразование строки wstring в строку в кодировке UTF-8.
	 *
	 * @param str строка wstring.
	 * @return строка в кодировке UTF-8.
	 */
	std::string wstring_to_utf8(const std::wstring& str);

	/*!
	 * @brief Преобразование строки в кодировке UTF-8 в строку u16string.
	 *
	 * @param str строка в кодировке UTF-8.
	 * @return строка u16string.
	 */
	std::u16string utf8_to_u16string(const std::string& str);

	/*!
	 * @brief Преобразование строки u16string в строку в кодировке UTF-8.
	 *
	 * @param str строка u16string.
	 * @return строка в кодировке UTF-8.
	 */
	std::string u16string_to_utf8 (const std::u16string& str);

	/*!
	 * @brief Получение строки с текущим временем в формате YYYY-MM-DD.HHMMSS.
	 *
	 * @return строка с текущим временем.
	 */
	std::string getFileTimestamp();

	/*!
	 * @brief Получение строки с текущим временем в формате YYYY-MM-DD HH:MM:SS.mmm.
	 *
	 * @return строка с текущим временем.
	 */
	std::string getLogTimestamp();

	/*!
	 * @brief Преобразование градусов в радианы.
	 *
	 * @param deg значение в градусах.
	 * @return значение в радианах.
	 */
	float deg2Radian(float deg);

	/*!
	 * @brief Преобразование метров в секунду в километры в час.
	 *
	 * @param ms значение в метрах в секунду.
	 * @return значение в километрах в час.
	 */
	float ms2kmh(float ms);

	/*!
	 * @brief Преобразование милей в час в километры в час.
	 *
	 * @param milih значение в милях в час.
	 * @return значение в километрах в час.
	 */
	float milih2kmh(float milih);

	/*!
	 * @brief Преобразование километров в час в метры в секунду.
	 *
	 * @param kmh значение в километрах в час.
	 * @return значение в метрах в секунду.
	 */
	float kmh2ms(float kmh);

	/*!
	 * @brief Преобразование километров в час в мили в час.
	 *
	 * @param kmh значение в километрах в час.
	 * @return значение в милях в час.
	 */
	float kmh2milih(float kmh);

	/*!
	 * @brief Удаление пробелов с начала и конца строки.
	 *
	 * @param str строка с пробелами.
	 * @return строка без пробелов.
	 */
	std::string allTrim(std::string str);

	/*!
	 * @brief Получение текущего времени используя строку формата.
	 *
	 * @param format строка формата совместимая с strftime.
	 * @return строка с текущим временем в указаном формате.
	 */
	std::string getCurrentTimeString(std::string format);

	/*!
	 * @brief Получение текущего времени.
	 *
	 * @return ссылка на структуру tm с текущим временем.
	 */
	struct tm *getCurrentTime();

	/*!
	 * Преобразование физических величин.
	 *
	 * @param phys физическая величина.
	 * @param from в каких единицах задано значение.
	 * @param to в какие единицы перевести.
	 * @param value заданное значение.
	 * @return преобразованное значение.
	 */
	float physConvert(Phys phys, Unit from, Unit to, float value);

	/*!
	 * @brief Форматирование числа с плавающей точкой.
	 *
	 * @param fmt формат в стиле printf.
	 * @param value число.
	 * @return строка с числом в заданном формате.
	 */
	std::string formatFloat(std::string fmt, float value);

	/*!
	 * @brief Форматирование времени заданном в секундах.
	 *
	 * @param sec время в секундах.
	 * @return строка с временем в формате HH:MM:SS.
	 */
	std::string formatTime(int sec);

	/*!
	 * @brief Проверка существования указанного пути.
	 *
	 * @param path путь в файловой системе.
	 * @return true - путь существует, false - путь отсутствует.
	 */
	bool pathExist(std::string path);

	/*!
	 * @brief Установка даты и времени в системе и в RTC.
	 *
	 * @param time значение даты и времени.
	 * @return код возврата, 0 - успех.
	 */
	int setDateTime(const struct tm *time);

	/*!
	 * @brief Проверка года на високосность.
	 *
	 * @param year значение года.
	 * @return true - високосный.
	 */
	bool isLeapYear(unsigned year);

	/*!
	 * @brief Название типа.
	 *
	 * @param type структура полученная вызовом typeid.
	 * @return строка с названием типа.
	 */
	std::string typeName(const std::type_info& type_inf);

	/*!
	 * @brief ФИО по полному имени.
	 *
	 * @param last_name фамилия.
	 * @param first_name имя.
	 * @param middle_name отчество.
	 * @return строка с названием типа.
	 */
	std::string getFio(const std::string last_name, const std::string first_name, const std::string middle_name);

	/*!
	 * @brief Ограничение числа в пределах.
	 *
	 * @param n число.
	 * @param lower нижний предел.
	 * @param upper верхний предел.
	 * @return число ограниченное в пределах lower <= n <= upper.
	 */
	template <typename T>
	constexpr T clamp(const T& n, const T& lower, const T& upper) {
	  return std::max(lower, std::min(n, upper));
	}

	std::vector<std::string> split(std::string str, char delimiter);
	std::string unsplit(std::vector<std::string> vec, char delimiter);

}

#endif /* SRC_UTILS_H_ */
