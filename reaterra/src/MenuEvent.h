#ifndef MENUEVENT_H_
#define MENUEVENT_H_

#include <memory>

#include "Event.h"
#include "Menu.h"

/// Событие перехода на меню.
class MenuEvent: public Event {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param menu ссылка на меню.
	 */
	MenuEvent(std::unique_ptr<Menu> menu);

	/// Деструктор.
	virtual ~MenuEvent();

	/*!
	 * Получение указателя на меню.
	 *
	 * @return указатель на меню.
	 */
	const std::unique_ptr<Menu>& getMenu() const;

private:

	std::unique_ptr<Menu> m_menu;

};

#endif /* MENUEVENT_H_ */
