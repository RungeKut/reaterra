#ifndef SETTINGSHRMENU_H_
#define SETTINGSHRMENU_H_

#include "Menu.h"
#include "Timer.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

/// Меню "подключение/отключение датчика ЧСС".
class SettingsHrMenu: public Menu {

public:

	/// Конструктор.
	SettingsHrMenu();

	/// Деструктор.
	virtual ~SettingsHrMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	static const int TABLE_ROWS = 9;
	static const int ROW_START_Y = 153;
	static const int ROW_STEP_Y = 66;

	Timer m_timer;
	std::string m_hrate;
	std::string m_battery;
	int m_search_cnt;
	int m_selected_row;
	std::shared_ptr<WidgetImageButton> m_search;
	std::shared_ptr<WidgetImageButton> m_search_ind;
	std::shared_ptr<WidgetImageButton> m_disconnect;
	std::vector<std::vector<std::shared_ptr<WidgetText>>> m_text_list;
	std::vector<std::shared_ptr<WidgetImageButton>> m_list;

	void updateTable();
	void stopSearch();
};

#endif /* SETTINGSHRMENU_H_ */
