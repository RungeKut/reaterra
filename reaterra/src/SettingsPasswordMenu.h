#ifndef SETTINGSPASSWORDMENU_H_
#define SETTINGSPASSWORDMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

/// Меню "смена пароля врача".
class SettingsPasswordMenu: public Menu {

public:

	/// Конструктор.
	SettingsPasswordMenu();

	/// Деструктор.
	virtual ~SettingsPasswordMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:
	WidgetImageButtonSP m_pwd;
	WidgetImageButtonSP m_save_button;
	WidgetImageButtonSP m_doc_pass;
	std::shared_ptr<WidgetText> m_pwd_text;
	std::wstring m_pwd_value;

};

#endif /* SETTINGSPASSWORDMENU_H_ */
