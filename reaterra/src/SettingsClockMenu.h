#ifndef SETTINGSCLOCKMENU_H_
#define SETTINGSCLOCKMENU_H_

#include "Menu.h"
#include "WidgetTextField.h"

/// Меню "настройка времени и даты".
class SettingsClockMenu: public Menu {

public:

	/// Конструктор.
	SettingsClockMenu();

	/// Деструктор.
	virtual ~SettingsClockMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:

	enum class Field {
		Hours,
		Minutes,
		Seconds,
		Days,
		Months,
		Years
	};

	static const unsigned DAYS[];

	Timer m_timer;
	struct tm m_current_time;
	int m_counter;
	WidgetTextFieldSP m_days;

	void daysCorrection(unsigned year);
};

#endif /* SETTINGSCLOCKMENU_H_ */
