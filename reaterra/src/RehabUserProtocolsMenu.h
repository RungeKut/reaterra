#ifndef REHABUSERPROTOCOLSMENU_H_
#define REHABUSERPROTOCOLSMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetText.h"

/// Меню "пользовательские профили/протоколы".
class RehabUserProtocolsMenu: public Menu {

public:

	/// Режим.
	enum class ProMode {
		Protocol,		///< Работа с протоколами.
		Profil,			///< Работа с профилями.
		ProfilSelect	///< Выбор профиля для пациента.
	};

	/*!
	 * @brief Конструктор.
	 */
	RehabUserProtocolsMenu();

	/// Деструктор.
	virtual ~RehabUserProtocolsMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:

	static const int TABLE_ROWS = 9;
	static const int ROW_START_Y = 94;
	static const int ROW_STEP_Y = 61;

	std::shared_ptr<WidgetImageButton> m_view;
	std::shared_ptr<WidgetImageButton> m_run;
	std::vector<std::shared_ptr<WidgetImageButton>> m_list;
	std::vector<std::vector<std::shared_ptr<WidgetText>>> m_text_list;
	DataSet m_protocols;
	int m_selected_row;
	int m_page_start;
	int m_user_protocol;
	std::shared_ptr<WidgetImageButton> m_pgup;
	std::shared_ptr<WidgetImageButton> m_pgdn;
	std::shared_ptr<WidgetRectangle> m_thumb;
	std::shared_ptr<WidgetImageButton> m_save;
	ProMode m_pro_mode;
	bool m_first_update;


	void updateTable();
	int getSelectedProtocolId();
	void initData();

};

#endif /* REHABUSERPROTOCOLSMENU_H_ */
