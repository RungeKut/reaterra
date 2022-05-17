#ifndef PATIENTEDITMENU_H_
#define PATIENTEDITMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "редактор базы пациентов".
class PatientEditMenu: public Menu {


public:

	/// Конструктор.
	PatientEditMenu();

	/// Деструктор.
	virtual ~PatientEditMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	static const int TABLE_ROWS = 10;
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
	std::shared_ptr<WidgetRectangle> m_thumb;
	std::shared_ptr<WidgetImageButton> m_delete;
	std::shared_ptr<WidgetImageButton> m_edit;
	bool m_first_update;

	void updateTable();
	int getSelectedPatientId();
	void recalculatePage(std::string id);
	std::string getSelectedPatientFio();
	void initData();

};

#endif /* PATIENTEDITMENU_H_ */
