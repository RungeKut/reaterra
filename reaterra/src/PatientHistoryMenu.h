#ifndef PATIENTHISTORYMENU_H_
#define PATIENTHISTORYMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "история пациентов".
class PatientHistoryMenu: public Menu {

public:

	/// Конструктор.
	PatientHistoryMenu();

	/// Деструктор.
	virtual ~PatientHistoryMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	static const int TABLE_ROWS = 16;
	static const int ROW_START_Y = 91;
	static const int ROW_STEP_Y = 37;

	std::vector<std::vector<std::vector<std::shared_ptr<WidgetTextField>>>> m_text_list;
	std::vector<std::vector<std::shared_ptr<WidgetTextField>>> m_title_list;
	std::vector<std::vector<std::shared_ptr<WidgetImageButton>>> m_bar_list;
	DataSet m_history;
	int m_page_start_v;
	size_t m_page_start_h;
	std::shared_ptr<WidgetImageButton> m_pgup_v;
	std::shared_ptr<WidgetImageButton> m_pgdn_v;
	std::shared_ptr<WidgetRectangle> m_thumb_v;
	std::shared_ptr<WidgetImageButton> m_pgup_h;
	std::shared_ptr<WidgetImageButton> m_pgdn_h;
	std::shared_ptr<WidgetRectangle> m_thumb_h;
	std::shared_ptr<WidgetImageButton> m_filter;
	std::vector<std::vector<int>> cols;

	std::string m_from_filter;
	std::string m_to_filter;
	std::string m_family_filter;

	int m_page_h_size;

	void updateTable();

};

#endif /* PATIENTHISTORYMENU_H_ */
