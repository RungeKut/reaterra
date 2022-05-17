#ifndef PATIENTSEARCHMENU_H_
#define PATIENTSEARCHMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "поиск пациентов".
class PatientSearchMenu: public Menu {

public:

	/// Конструктор.
	PatientSearchMenu();

	/// Деструктор.
	virtual ~PatientSearchMenu();


private:

	static const int TABLE_ROWS = 9;
	static const int TABLE_COLS = 6;
	static const int ROW_START_Y = 159;
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
	std::shared_ptr<WidgetImageButton> m_set;
	std::shared_ptr<WidgetImageButton> m_unset;
	std::string m_filter;

	void updateTable();
	int getSelectedPatientId();
	int getSelectedPatientProfile();
	std::string getSelectedPatientFio();
	float getSelectedPatientWeight();

};

#endif /* PATIENTSEARCHMENU_H_ */
