#ifndef PATIENTSTATSMENU_H_
#define PATIENTSTATSMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Меню "статистика по учетной записи пациента".
class PatientStatsMenu: public Menu {


public:

	/// Конструктор.
	PatientStatsMenu();

	/// Деструктор.
	virtual ~PatientStatsMenu();


private:

	DataSet m_patient;
	DataSet m_statistic;
	std::shared_ptr<WidgetImageButton> m_set;
	std::shared_ptr<WidgetImageButton> m_unset;

};

#endif /* PATIENTSTATSMENU_H_ */
