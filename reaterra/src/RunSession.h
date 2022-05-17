#ifndef RUNSESSION_H_
#define RUNSESSION_H_

#include <chrono>
#include <string>

#include "TreadmillDriver.h"

/// Беговая сессия.
class RunSession {

public:

	/// Конструктор.
	RunSession();

	/// Деструктор.
	virtual ~RunSession();

	/*!
	 * @brief Начало сессии.
	 *
	 * @param patient_id идентификатор пациента.
	 * @param profile название вида бега.
	 * @param sas true: бег с САС.
	 */
	void start(const int patient_id, const std::string profile, std::string sas = "");

	/// Завершение сессии.
	void finish();

	/*!
	 * @brief Проверка состояния сессии.
	 *
	 * @return true: сессия запущена, false: не запущена.
	 */
	bool isStarted() const;

	/// Увеличение времени сессии на 1 секунду.
	void incSec();

	/*!
	 * @brief Получение длительности сессии.
	 *
	 * @return длительность сессии в секундах.
	 */
	int getDuration() const;

	/*!
	 * @brief Получение пройденного расстояния.
	 *
	 * @return пройденное расстояние в км.
	 */
	float getDistance() const;

	/*!
	 * @brief Получение затраченной энергии.
	 *
	 * @return затраченная энергия в джоулях.
	 */
	float getEnergy() const;

	/*!
	 * @brief Получение аэробной мощности.
	 *
	 * @return аэробная мощность в мл/мин/кг.
	 */
	float getPowerAero() const;

	/*!
	 * @brief Получение механической мощности.
	 *
	 * @return механическая мощность в ваттах.
	 */
	float getPowerMech() const;

	/*!
	 * @brief Получение текущего угла наклона.
	 *
	 * @return угол наклона в градусах.
	 */
	float getAngle() const;

	/*!
	 * @brief Получение скорости дорожки.
	 *
	 * @return скорость дорожки в км/ч.
	 */
	float getSpeed() const;

	/*!
	 * @brief Получение расстояния в виде строки.
	 *
	 * @param dist расстояние.
	 * @return отформатированная строка.
	 */
	std::string getDistanceString(float dist = -1) const;

	/*!
	 * @brief Получение угла наклона в виде строки.
	 *
	 * @param angle угол наклона.
	 * @param format формат отображения.
	 * @return отформатированная строка.
	 */
	std::string getAngleString(float angle = -1, std::string format = "") const;

	/*!
	 * @brief Получение скорости в виде строки.
	 *
	 * @param speed скорость.
	 * @param format формат отображения.
	 * @return отформатированная строка.
	 */
	std::string getSpeedString(float speed = -1, std::string format = "") const;

	/*!
	 * @brief Получение энергии в виде строки.
	 *
	 * @param energy энергия.
	 * @return отформатированная строка.
	 */
	std::string getEnergyString(float energy = -1) const;

	/*!
	 * @brief Получение аэробной мощности в виде строки.
	 *
	 * @param value аэробная мощность.
	 * @return отформатированная строка.
	 */
	std::string getPowerAeroString(float const value = -1) const;

	/*!
	 * @brief Получение механической мощности в виде строки.
	 *
	 * @param value механическая мощность.
	 * @return отформатированная строка.
	 */
	std::string getPowerMechString(float const value = -1) const;

	/*!
	 * @brief Получение форматной строки для скорости.
	 *
	 * @return форматная строка.
	 */
	std::string getSpeedFormat() const;

	/*!
	 * @brief Получение форматной строки для угла.
	 *
	 * @return форматная строка.
	 */
	std::string getAngleFormat() const;

	/*!
	 * @brief Получение форматной строки для механической мощности.
	 *
	 * @return форматная строка.
	 */
	std::string getPowerMechFormat() const;

	/*!
	 * @brief Получение ускорения в виде строки.
	 *
	 * @param accel ускорение.
	 * @return отформатированная строка.
	 */
	std::string getAccelString(float accel = -1) const;

	/*!
	 * @brief Получение скорости в км/ч.
	 *
	 * @param value скорость.
	 * @return скорость в км/ч.
	 */
	float getSpeedKmH(const float value);

	/*!
	 * @brief Получение угла в градусах.
	 *
	 * @param value угол.
	 * @return угол в градусах.
	 */
	float getAngleDegree(const float value);

	/*!
	 * @brief Получение мощности в ваттах.
	 *
	 * @param value мощность.
	 * @return мощность в ваттах.
	 */
	float getPowerWatt(const float value);

	/*!
	 * @brief Получение текущего значения пульса.
	 *
	 * @return пульс.
	 */
	int getHrate() const;

	/*!
	 * @brief Получение идентификатора текущего пациента.
	 *
	 * @return идентификатор пациента.
	 */
	int getPatientId() const;

	/*!
	 * @brief Получение названия вида бега текущей сессии.
	 *
	 * @return название вида бега.
	 */
	std::string getProfile() const;

	/*!
	 * @brief Получение средней скорости.
	 *
	 * @return средняя скорость.
	 */
	float getSpeedAvg() const;

	/*!
	 * @brief Получение максимального ускорения.
	 *
	 * @return максимальное ускорение.
	 */
	float getAccelMax() const;

	/*!
	 * @brief Получение максимального угла наклона.
	 *
	 * @return максимальный угол наклонаю
	 */
	float getAngleMax() const;

	/*!
	 * @brief Получение минимального значения пульса.
	 *
	 * @return минимальный пульс.
	 */
	int getHrateMin() const;

	/*!
	 * @brief Получение максимального значения пульса.
	 *
	 * @return максимальный пульс.
	 */
	int getHrateMax() const;

	/*!
	 * @brief Получение средней аэробной мощности.
	 *
	 * @return средняя аэробная мощность.
	 */
	float getPowerAeroAvg() const;

	/*!
	 * @brief Получение средней механической мощности.
	 *
	 * @return средняя механическая мощность.
	 */
	float getPowerMechAvg() const;

	/*!
	 * @brief Получение среднего значения пульса.
	 *
	 * @return средний пульс.
	 */
	float getHrateAvg() const;

	/*!
	 * @brief Получение текущего ускорения.
	 *
	 * @return текущее ускорение.
	 */
	float getAcceleration() const;


	/*!
	 * @brief Получение мощности по параметрам.
	 *
	 * @param speed скорость.
	 * @param angle угол.
	 * @param weight вес пациента.
	 * @param direction направление движения.
	 * @return мощность в ваттах.
	 */
	float getPowerMechByParams(float speed,  float angle, float weight, TreadmillDriver::Direction direction = TreadmillDriver::Direction::Forward);


	/*!
	 * @brief САС. Получение текущей симметричности по левой ноге.
	 *
	 * @return симметричность левой ноги.
	 */
	int getSasSymmetryLeft() const;


	/*!
	 * @brief САС. Получение текущей симметричности по правой ноге.
	 *
	 * @return симметричность правой ноги.
	 */
	int getSasSymmetryRight() const;


	/*!
	 * @brief САС. Получение средней симметричности по левой ноге.
	 *
	 * @return средняя симметричность левой ноги.
	 */
	int getSasSymmetryLeftAvg() const;


	/*!
	 * @brief САС. Получение средней симметричности по правой ноге.
	 *
	 * @return средняя симметричность правой ноги.
	 */
	int getSasSymmetryRightAvg() const;


	/*!
	 * @brief САС. Получение текущей длины шага.
	 *
	 * @return длина шага.
	 */
	int getSasStepLength() const;


	/*!
	 * @brief САС. Получение средней длины шага.
	 *
	 * @return средняя длина шага.
	 */
	int getSasStepLengthAvg() const;


	/// Инициализация сессии.
	void init();


	/*!
	 * @brief САС. Получение режима САС.
	 *
	 * @return режим САС.
	 */
	std::string getSasMode();


private:

	bool m_started;
	std::chrono::steady_clock::time_point m_start_time;
	std::chrono::steady_clock::time_point m_finish_time;
	int m_patient_id;
	std::string m_profile_name;
	int m_duration;
	double m_distance;
	float m_max_accel;
	float m_max_angle;

	float m_power_mech;
	float m_power_aero;
	float m_energy;
	int m_hrate;
	int m_min_hrate;
	int m_max_hrate;
	double m_speed_sum;
	double m_power_mech_sum;
	double m_power_aero_sum;
	double m_hrate_sum;

	double m_sas_sym_left_sum;
	double m_sas_sym_right_sum;
	double m_sas_step_length_sum;

	std::string m_sas_mode;



};

#endif /* RUNSESSION_H_ */
