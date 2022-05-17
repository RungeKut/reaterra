#ifndef MODALAUTHDOCTOR_H_
#define MODALAUTHDOCTOR_H_

#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Диалог ввода пароля доктора.
class ModalAuthDoctor: public Menu {

public:

	/// Конструктор.
	ModalAuthDoctor();

	/// Деструктор.
	virtual ~ModalAuthDoctor();


private:

	std::shared_ptr<WidgetTextField> m_pwd_wrong;
	std::shared_ptr<WidgetTextField> m_pwd_entry;

};

#endif /* MODALAUTHDOCTOR_H_ */
