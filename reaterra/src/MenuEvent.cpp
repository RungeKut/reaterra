#include "MenuEvent.h"

MenuEvent::MenuEvent(std::unique_ptr<Menu> menu)
:	Event(Event::EventType::Menu),
	m_menu {std::move(menu)}
{
}

MenuEvent::~MenuEvent() {
}

const std::unique_ptr<Menu>& MenuEvent::getMenu() const {
	return m_menu;
}
