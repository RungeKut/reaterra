#ifndef SETTINGSREGISTERSMENU_H_
#define SETTINGSREGISTERSMENU_H_

#include "Menu.h"

#include "TreadmillDriver.h"

/// Меню "системная информация".
class SettingsRegistersMenu: public Menu {

public:

	/// Конструктор.
	SettingsRegistersMenu();

	/// Деструктор.
	virtual ~SettingsRegistersMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	Timer m_timer;
	std::map<TreadmillDriver::Reg, uint16_t> m_reg_prev;

};

#endif /* SETTINGSREGISTERSMENU_H_ */
