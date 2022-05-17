#ifndef REHABPROTOCOLSELECTIONMENU_H_
#define REHABPROTOCOLSELECTIONMENU_H_

#include "Context.h"
#include "Menu.h"

/// Меню "выбор протокола тестирования".
class RehabProtocolSelectionMenu: public Menu {

public:

	/// Конструктор.
	RehabProtocolSelectionMenu();

	/// Деструктор.
	virtual ~RehabProtocolSelectionMenu();


private:

	DataSet m_protocols;

};

#endif /* REHABPROTOCOLSELECTIONMENU_H_ */
