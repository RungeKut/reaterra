#ifndef TESTMENU_H_
#define TESTMENU_H_

#include "Menu.h"

///Меню тестирования панели
class TestMenu: public Menu {

public:

	/// Конструктор
	TestMenu();
	/// Деструктор
	virtual ~TestMenu();
	/// Обработчик событий
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:
	Timer m_timer;
	int m_cnt;
	std::shared_ptr<WidgetTextField> m_message;
	std::string m_color;
	std::string m_touch;
	std::string m_flash;

};

#endif /* TESTMENU_H_ */
