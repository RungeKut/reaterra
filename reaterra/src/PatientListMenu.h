#ifndef PATIENTLISTMENU_H_
#define PATIENTLISTMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "база пациентов".
class PatientListMenu: public Menu {


public:

	/// Конструктор.
	PatientListMenu();

	/// Деструктор.
	virtual ~PatientListMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	static const int TABLE_ROWS = 9;
	static const int TABLE_COLS = 6;
	static const int ROW_START_Y = 97;
	static const int ROW_STEP_Y = 55;

	std::vector<std::vector<std::shared_ptr<WidgetTextField>>> m_text_list;
	std::vector<std::shared_ptr<WidgetImageButton>> m_list;
	std::string m_order_field;
	bool m_order_dir;
	DataSet m_patients;
	int m_selected_row;
	int m_page_start;
	std::shared_ptr<WidgetImageButton> m_pgup;
	std::shared_ptr<WidgetImageButton> m_pgdn;
	std::shared_ptr<WidgetImageButton> m_search;
	std::shared_ptr<WidgetImageButton> m_details;
	std::shared_ptr<WidgetImageButton> m_stat;
	std::shared_ptr<WidgetRectangle> m_thumb;
	std::shared_ptr<WidgetImageButton> m_set;
	std::shared_ptr<WidgetImageButton> m_unset;
	bool m_first_update;

	void updateTable();
	int getSelectedPatientId();
	void saveId();
	void initData();

};

#endif /* PATIENTLISTMENU_H_ */
