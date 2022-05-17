#ifndef PATIENTDETAILSMENU_H_
#define PATIENTDETAILSMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Меню "детальная иформация о пациенте".
class PatientDetailsMenu: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param id идентификатор пациента.
	 */
	PatientDetailsMenu();

	/// Деструктор.
	virtual ~PatientDetailsMenu();


private:

	DataSet m_patient;
	std::shared_ptr<WidgetImageButton> m_set;
	std::shared_ptr<WidgetImageButton> m_unset;

};

#endif /* PATIENTDETAILSMENU_H_ */
