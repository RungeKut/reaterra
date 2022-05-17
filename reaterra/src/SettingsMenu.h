#ifndef SETTINGSMENU_H_
#define SETTINGSMENU_H_

#include <string>

#include "Menu.h"

/// Меню "настройки".
class SettingsMenu: public Menu {

public:

	/// Конструктор.
	SettingsMenu(std::string user = {});

	/// Деструктор.
	virtual ~SettingsMenu();

};

#endif /* SETTINGSMENU_H_ */
