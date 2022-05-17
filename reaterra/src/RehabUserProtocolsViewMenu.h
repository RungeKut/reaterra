#ifndef REHABUSERPROTOCOLSVIEWMENU_H_
#define REHABUSERPROTOCOLSVIEWMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "MenuEvent.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "просмотр/создание/редактирование профиля/протокола".
class RehabUserProtocolsViewMenu: public Menu {

public:

	/// Режим.
	enum class Mode {
		View,		///< Просмотр.
		New,		///< Создание.
		Edit		///< Редактирование.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param id идентификатор протокола/профиля.
	 * @param mode режим просмотр/создание/редактирование.
	 * @param pro_mode выбор профиль/протокол.
	 */
	RehabUserProtocolsViewMenu();

	/// Деструктор.
	virtual ~RehabUserProtocolsViewMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:

	static const int TABLE_ROWS = 8;
	static const int TABLE_COLS = 5;
	static const int ROW_START_Y = 243;
	static const int ROW_STEP_Y = 63;

	std::vector<std::vector<std::shared_ptr<WidgetTextField>>> m_text_list;
	DataSet m_intervals;
	int m_selected_row;
	int m_page_start;
	std::shared_ptr<WidgetTextField> m_name;
	std::shared_ptr<WidgetTextField> m_description;
	std::shared_ptr<WidgetTextField> m_speed_title;
	std::shared_ptr<WidgetTextField> m_duration_title;
	std::shared_ptr<WidgetTextField> m_angle_title;
	DataSet m_params;
	std::shared_ptr<WidgetImageButton> m_pgup;
	std::shared_ptr<WidgetImageButton> m_pgdn;
	std::shared_ptr<WidgetRectangle> m_thumb;
	std::shared_ptr<WidgetImageButton> m_add_interval;
	std::shared_ptr<WidgetImageButton> m_delete_interval;
	Mode m_mode;
	bool m_changed;
	std::string m_prev_name;
	RehabUserProtocolsMenu::ProMode m_pro_mode;
	int m_col_cor;
	std::unique_ptr<Menu> m_next_menu;

	std::unique_ptr<MenuEvent> backAction(std::unique_ptr<Menu> menu, std::string subject);
	void updateTable();

};

#endif /* REHABUSERPROTOCOLSVIEWMENU_H_ */
