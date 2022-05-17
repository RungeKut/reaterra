#ifndef SETTINGSUNITSMENU_H_
#define SETTINGSUNITSMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"

/// Меню "единицы измерения физических величин".
class SettingsUnitsMenu: public Menu {

public:

	/// Конструктор.
	SettingsUnitsMenu();

	/// Деструктор.
	virtual ~SettingsUnitsMenu();

private:

	std::string m_dist_selected;
	std::shared_ptr<WidgetImageButton> m_dist_km;
	std::shared_ptr<WidgetImageButton> m_dist_m;
	std::shared_ptr<WidgetImageButton> m_dist_mile;
	std::string m_speed_selected;
	std::shared_ptr<WidgetImageButton> m_speed_kmh;
	std::shared_ptr<WidgetImageButton> m_speed_ms;
	std::shared_ptr<WidgetImageButton> m_speed_mh;
	std::string m_angle_selected;
	std::shared_ptr<WidgetImageButton> m_angle_gr;
	std::shared_ptr<WidgetImageButton> m_angle_per;
	std::string m_energy_selected;
	std::shared_ptr<WidgetImageButton> m_energy_kkal;
	std::shared_ptr<WidgetImageButton> m_energy_j;
	std::shared_ptr<WidgetImageButton> m_energy_kwh;
	std::string m_pwr_selected;
	std::shared_ptr<WidgetImageButton> m_pwr_kw;
	std::shared_ptr<WidgetImageButton> m_pwr_w;
	std::string m_pwrair_selected;
	std::shared_ptr<WidgetImageButton> m_pwrair_met;
	std::shared_ptr<WidgetImageButton> m_pwrair_mmk;

};

#endif /* SETTINGSUNITSMENU_H_ */
