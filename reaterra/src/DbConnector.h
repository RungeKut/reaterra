#ifndef DBCONNECTOR_H_
#define DBCONNECTOR_H_

#include <stdexcept>
#include <string>
#include <vector>
#include <mariadb/mysql.h>

#include "RunSession.h"
#include "Utils.h"

/// Ошибка БД.
class DbError: public std::runtime_error {

public:

	unsigned int error_code; ///< Код ошибки.

	/*!
	 * Конструктор.
	 *
	 * @param what_arg описание ошибки.
	 * @param code (опционально) код ошибки. 0 по умолчанию.
	 */
	explicit DbError(const std::string& what_arg, unsigned int code = 0) : std::runtime_error(what_arg), error_code {code} {}

};

/// Набор данных.
typedef std::vector<std::vector<std::string>> DataSet;

/// Интерфейс БД.
class DbConnector {

public:

	/// Параметры протоколов.
	enum class ProtocolParam {
		Type = 0,			///< Тип.
		Name = 1,			///< Название.
		Modification = 2,	///< Модификация.
		DurationUnit = 3,	///< Единицы времени.
		DurationFmt = 4,	///< Формат времени.
		SpeedUnit = 5,		///< Единицы скорости.
		SpeedFmt = 6,		///< Формат скорости.
		AngleUnit = 7,		///< Единицы угла наклона.
		AngleFmt = 8,		///< Формат угла наклона.
		Description = 9		///< Описание.
	};

	/// Параметры интервала.
	enum class IntervalData {
		Name = 0,			///< Номер интервала.
		Duration = 1,		///< Длительность.
		Speed = 2,			///< Скорость.
		Angle = 3,			///< Угол наклона.
		Type = 4,			///< Тип.
		Acceleration = 5	///< Ускорение.
	};

	/// Параметры протокола.
	enum class ProtocolList {
		Name = 0,			///< Название.
		Id = 1,				///< Идентификатор.
		Description = 2		///< Описание.
	};

	/// Параметры пациента.
	enum class PatientList {
		Id = 0,				///< Идентификатор.
		Number = 1,			///< Номер.
		LastName = 2,		///< Фамилия.
		FirstName = 3,		///< Имя.
		MiddleName = 4,		///< Отчество.
		Sex = 5,			///< Пол.
		Age = 6,			///< Возраст.
		Diagnosis = 7,		///< Диагноз.
		Doctor = 8,			///< Лечащий врач.
		Weight = 9,			///< Вес.
		ProfileId = 10,		///< Идентификатор профиля.
		Profile = 11		///< Наименование профиля.
	};

	/// Параметры статистики.
	enum class Statistic {
		Fio = 0,			///< ФИО.
		Distance = 1,		///< Пройденное расстояние.
		Duration = 2,		///< Потраченное время.
		Energy = 3,			///< Затраченная энергия.
		Acceleration = 4,	///< Максимальное ускорение.
		HRateMin = 5,		///< Минимальный пульс.
		HRateMax = 6		///< Максимальный пульс.
	};

	/// Параметры истории.
	enum class History {
		Fio = 0,			///< ФИО.
		Time = 1,			///< Потраченное время.
		Profile = 2,		///< Название профиля.
		Distance = 3,		///< Пройденное расстояние.
		Duration = 4,		///< Потраченное время.
		Speed = 5,			///< Средняя скорость.
		Acceleration = 6,	///< Максимальное ускорение.
		Angle = 7,			///< Максимальный угол наклона.
		Energy = 8,			///< Затраченная энергия.
		PowerMech = 9,		///< Средняя механическая мощность.
		PowerAero = 10,		///< Средняя аэробная мощность.
		HRate = 11,			///< Средний пульс.
		LastName = 12,		///< Фамилия.
		FirstName = 13,		///< Имя.
		MiddleName = 14,	///< Отчество.
		TimeStamp = 15,		///< Время записи.
		SasStepLength = 16,			///< Средняя длина шага в режиме САС.
		SasSymmetryLeft = 17,		///< Симметрия левой ноги в режиме САС.
		SasSymmetryRight = 18,		///< Симметрия правой ноги в режиме САС.
		SasMode = 19,				///< Режим САС.
	};

	/// Конструктор.
	DbConnector();
	/// Деструктор.
	virtual ~DbConnector();

	/*!
	 * @brief Получение списка протоколов.
	 *
	 * @param type тип протоколов ("ПРОТ10"/"USER"/"USER_PROF").
	 * @return список протоколов.
	 */
	DataSet getProtocols(std::string type);

	/*!
	 * @brief Получение модификаций протокола.
	 *
	 * @param id идентификатор протокола.
	 * @return список модификаций.
	 */
	DataSet getProtocolModifs(int id);

	/*!
	 * @brief Получение параметров протокола.
	 *
	 * @param id идентификатор протокола.
	 * @return параметры протокола.
	 */
	DataSet getProtocolParams(int id);


	/*!
	 * @brief Получение интервалов протокола.
	 *
	 * @param id идентификатор протокола.
	 * @return интервалы протокола.
	 */
	DataSet getIntervals(int id);

	/*!
	 * @brief Запрос параметра.
	 *
	 * @param name имя параметра.
	 * @param def значение по умолчанию.
	 * @return значение параметра.
	 */
	std::string getParam(std::string name, std::string def = "no_insert");

	/*!
	 * @brief Установка параметра.
	 *
	 * @param name имя параметра.
	 * @param value значение параметра.
	 */
	void setParam(std::string name, std::string value);

	/*!
	 * @brief Вставка нового протокола.
	 *
	 * @param protocol парамтры протокола.
	 * @param intervals параметры интервалов.
	 * @param pro_type тип протокола.
	 * @return идентификатор протокола или -1 в случае ошибки.
	 */
	int insertProtocol(DataSet protocol, DataSet intervals, std::string pro_type);

	/*!
	 * @brief Обновление данных протокола.
	 *
	 * @param id идентификатор протокола.
	 * @param protocol параметры протокола.
	 * @param intervals интервалы протокола.
	 */
	void updateProtocol(int id, DataSet protocol, DataSet intervals);

	/*!
	 * @brief Удаление протокола.
	 *
	 * @param id идентификатор удаляемого протокола.
	 */
	void deleteProtocol(int id);

	/*!
	 * @brief Удаление пациента.
	 *
	 * @param id идентификатор удаляемого пациента.
	 */
	void deletePatient(int id);

	/// Создание резервной копии БД.
	std::string backupDB();

	/*!
	 * @brief Восстановление БД.
	 *
	 * @param gz_dump путь к файлу резервной копии.
	 */
	void restoreDB(std::string gz_dump);

	/*!
	 * @brief Получение значения единиц измерения по названию.
	 *
	 * @param name название единицы измерения.
	 * @return значение перечисления.
	 */
	Utils::Unit getParamEnum(std::string name);

	/*!
	 * @brief Получение списка пациентов.
	 *
	 * @param order поле сортировки.
	 * @param asc направление сортировки.
	 * @param filter фильтр записей.
	 * @return список пациентов.
	 */
	DataSet getPatients(std::string order, bool asc, std::string filter = "");

	/*!
	 * @brief Получение данных пациента.
	 *
	 * @param id идентификатор пациента.
	 * @return данные пациента.
	 */
	DataSet getPatient(int id);

	/*!
	 * @brief Добавление пациента.
	 *
	 * @param patient данные пациента.
	 * @return идентификатор пациента или -1 в случае ошибки
	 */
	int insertPatient(DataSet patient);

	/*!
	 * @brief Обновление данных пациента.
	 *
	 * @param patient данные пациента.
	 */
	void updatePatient(DataSet patient);

	/*!
	 * @brief Формирование текстового дампа из набора данных.
	 *
	 * @param ds набор данных.
	 * @param name пометка.
	 * @return текст дампа.
	 */
	static std::string dumpDataSet(DataSet ds, std::string name = "");

	/*!
	 * @brief Добавление записи в историю.
	 *
	 * @param session ссылка на беговую сессию.
	 */
	void addRunSession(RunSession &session);

	/*!
	 * @brief Получение статистики.
	 *
	 * @param patient_id идентификатор пациента.
	 * @return данные статистики.
	 */
	DataSet getStatistic(int patient_id);

	/*!
	 * @brief Получение истории.
	 *
	 * @param from начало периода.
	 * @param to конец периода.
	 * @param fio фильтр по фамилии пациента.
	 * @return данные истории.
	 */
	DataSet getHistory(std::string from = "", std::string to = "", std::string fio = "");

	/*!
	 * @brief Экспорт данных в формате Weintek.
	 *
	 * @param file название файла для экспорта.
	 */
	void exportToEmi(std::string file);

	/*!
	 * @brief Импорт данных в формате Weintek.
	 *
	 * @param file название файла для импорта.
	 */
	void importFromEmi(std::string file);

private:

	  MYSQL *m_mysql;
	  bool m_init_ok;

	  DataSet select(std::string query);
	  void command(std::string cmd);
	  void upgrade();

};

#endif /* DBCONNECTOR_H_ */
