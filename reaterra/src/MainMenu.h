#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "Menu.h"

/// Главное меню приложения.
class MainMenu : public Menu {

public:

	/// Конструктор.
	MainMenu();

	/// Деструктор.
	virtual ~MainMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;
};

#endif /* MAINMENU_H_ */
