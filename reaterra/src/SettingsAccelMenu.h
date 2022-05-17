#ifndef SETTINGSACCELMENU_H_
#define SETTINGSACCELMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Меню "задание ускорения и торможения".
class SettingsAccelMenu: public Menu {

public:

	/// Конструктор.
	SettingsAccelMenu();

	/// Деструктор.
	virtual ~SettingsAccelMenu();


private:

	std::shared_ptr<WidgetImageButton> m_accel_1;
	std::shared_ptr<WidgetImageButton> m_accel_2;
	std::shared_ptr<WidgetImageButton> m_accel_3;
	std::shared_ptr<WidgetImageButton> m_accel_4;
	std::shared_ptr<WidgetImageButton> m_accel_5;
	std::shared_ptr<WidgetImageButton> m_accel_6;
	std::shared_ptr<WidgetImageButton> m_accel_7;
	std::shared_ptr<WidgetImageButton> m_decel_1;
	std::shared_ptr<WidgetImageButton> m_decel_2;
	std::shared_ptr<WidgetImageButton> m_decel_3;
	std::shared_ptr<WidgetImageButton> m_decel_4;
	std::shared_ptr<WidgetImageButton> m_decel_5;
	std::shared_ptr<WidgetImageButton> m_decel_6;
	std::shared_ptr<WidgetImageButton> m_decel_7;
	std::shared_ptr<WidgetImageButton> m_accel_chart;
	std::shared_ptr<WidgetImageButton> m_decel_chart;
	int m_accel_prev;
	int m_decel_prev;

	void updateAccel();
	void updateDecel();

};

#endif /* SETTINGSACCELMENU_H_ */
