#ifndef MODALPATIENTHISTORYFILTER_H_
#define MODALPATIENTHISTORYFILTER_H_

#include "Menu.h"
#include "WidgetTextField.h"

/// Окно установки фильтра истории.
class ModalPatientHistoryFilter: public Menu {

public:

	/// Конструктор.
	ModalPatientHistoryFilter();

	/// Деструктор.
	virtual ~ModalPatientHistoryFilter();


private:

	std::shared_ptr<WidgetTextField> m_family_entry;
	std::shared_ptr<WidgetTextField> m_from_entry;
	std::shared_ptr<WidgetTextField> m_to_entry;

};

#endif /* MODALPATIENTHISTORYFILTER_H_ */
