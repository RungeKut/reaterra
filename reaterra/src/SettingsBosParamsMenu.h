#ifndef SETTINGSBOSPARAMSMENU_H_
#define SETTINGSBOSPARAMSMENU_H_

#include "Menu.h"

/// Меню "Настройка параметров режимов с БОС".
class SettingsBosParamsMenu: public Menu {

public:

	/// Вектор значений по умолчанию нижней границы.
	static std::vector<std::string> getParamsLow();

	/// Вектор значений по умолчанию верхней границы.
	static std::vector<std::string> getParamsHigh();

	/*!
	 * @brief Получение дельты скорости из таблицы параметров.
	 *
	 * @param data вектор значений.
	 * @param row индекс ряда.
	 * @return значение дельты скорости.
	 */
	static float getSpeed(std::vector<std::string> data, int row);

	/*!
	 * @brief Получение дельты угла из таблицы параметров.
	 *
	 * @param data вектор значений.
	 * @param row индекс ряда.
	 * @return значение дельты угла.
	 */
	static float getAngle(std::vector<std::string> data, int row);

	/*!
	 * @brief Получение длительности интервала из таблицы параметров.
	 *
	 * @param data вектор значений.
	 * @param row индекс ряда.
	 * @return значение длительности интервала.
	 */
	static float getTime(std::vector<std::string> data, int row);

	/// Конструктор.
	SettingsBosParamsMenu();

	/// Деструктор.
	virtual ~SettingsBosParamsMenu();


private:

	static const std::vector<std::string> getDefaultLow();
	static const std::vector<std::string> getDefaultHigh();
	std::vector<std::string> m_bos_params_low;
	std::vector<std::string> m_bos_params_high;

};

#endif /* SETTINGSBOSPARAMSMENU_H_ */
