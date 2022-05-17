#ifndef MODALAUTHADMIN_H_
#define MODALAUTHADMIN_H_

#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

/// Диалог ввода пароля доктора/администратора.
class ModalAuthAdmin: public Menu {

public:

	/// Конструктор.
	ModalAuthAdmin();

	/// Деструктор.
	virtual ~ModalAuthAdmin();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(EventUP event) override;


private:

	bool m_admin_selected;
	std::shared_ptr<WidgetImageButton> m_admin;
	std::shared_ptr<WidgetImageButton> m_doc;
	std::shared_ptr<WidgetImageButton> m_pwd;
	std::shared_ptr<WidgetText> m_pwd_text;
	std::shared_ptr<WidgetText> m_pwd_wrong_text;
	std::wstring m_pwd_value;

};

#endif /* MODALAUTHADMIN_H_ */
