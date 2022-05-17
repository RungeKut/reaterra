#ifndef REHABUSERPROTOCOLSEDITMENU_H_
#define REHABUSERPROTOCOLSEDITMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "редактирование профиля/протокола".
class RehabUserProtocolsEditMenu: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 */
	RehabUserProtocolsEditMenu();

	/// Деструктор.
	virtual ~RehabUserProtocolsEditMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	static const int TABLE_ROWS = 8;
	static const int ROW_START_Y = 171;
	static const int ROW_STEP_Y = 59;

	std::shared_ptr<WidgetImageButton> m_modify;
	std::shared_ptr<WidgetImageButton> m_delete;
	std::vector<std::shared_ptr<WidgetImageButton>> m_list;
	std::vector<std::vector<std::shared_ptr<WidgetTextField>>> m_text_list;
	DataSet m_protocols;
	int m_selected_row;
	int m_page_start;
	std::shared_ptr<WidgetImageButton> m_pgup;
	std::shared_ptr<WidgetImageButton> m_pgdn;
	std::shared_ptr<WidgetRectangle> m_thumb;
	RehabUserProtocolsMenu::ProMode m_pro_mode;
	bool m_first_update;


	void updateTable();
	int getSelectedProtocolId();
	void recalculatePage(std::string id);
	void initData();

};

#endif /* REHABUSERPROTOCOLSEDITMENU_H_ */
