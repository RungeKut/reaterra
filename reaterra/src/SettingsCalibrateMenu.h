#ifndef SETTINGSCALIBRATEMENU_H_
#define SETTINGSCALIBRATEMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Меню "калибровка положения беговой дорожки".
class SettingsCalibrateMenu: public Menu {

public:

	/// Конструктор.
	SettingsCalibrateMenu();

	/// Деструктор.
	virtual ~SettingsCalibrateMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	WidgetTextFieldSP m_message;
	WidgetTextFieldSP m_angle_msg;
	WidgetImageButtonSP m_negative;
	WidgetImageButtonSP m_positive;

	const static int DELTA_ANGLE = 10;

	Timer m_timer;
	int m_angle;

};

#endif /* SETTINGSCALIBRATEMENU_H_ */
