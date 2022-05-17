#ifndef SETTINGSEXTRACALIBACTUATORMENU_H_
#define SETTINGSEXTRACALIBACTUATORMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"

/// Меню "калибровка подъемника".
class SettingsExtraCalibActuatorMenu: public Menu {


public:

	/// Конструктор.
	SettingsExtraCalibActuatorMenu();

	/// Деструктор.
	virtual ~SettingsExtraCalibActuatorMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	const static int DELTA_ANGLE = 10;

	WidgetTextFieldSP m_angle_msg;
	WidgetImageButtonSP m_negative;
	WidgetImageButtonSP m_positive;

	Timer m_timer;
	int m_angle;

};

#endif /* SETTINGSEXTRACALIBACTUATORMENU_H_ */
