#ifndef SASSERVICEMENU_H_
#define SASSERVICEMENU_H_

#include "Menu.h"

/// Меню для отображения сервисной информации о состоянии датчиков САС.
class SasServiceMenu: public Menu {

public:

	/// Конструктор
	SasServiceMenu();

	/// Деструктор
	virtual ~SasServiceMenu();

	/// Обработчик событий
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	WidgetSPVec m_sensors;
	WidgetTextFieldSP m_message;
	WidgetSP m_stat_btn;

	void updateSensors();

};

#endif /* SASSERVICEMENU_H_ */
