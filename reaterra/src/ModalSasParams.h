#ifndef MODALSASPARAMS_H_
#define MODALSASPARAMS_H_

#include "Menu.h"

/// Окно установки параметров предела пульса.
class ModalSasParams: public Menu {

public:

	/// Конструктор.
	ModalSasParams();

	/// Деструктор.
	virtual ~ModalSasParams();

private:

	std::string m_alarm_flag;
	WidgetTextFieldSP m_hr_max;
	WidgetTextFieldSP m_hr_crit;
	WidgetImageButtonSP m_hr_alarm;

};

#endif /* MODALSASPARAMS_H_ */
