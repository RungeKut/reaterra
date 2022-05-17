#ifndef SETTINGSEXPORTMENU_H_
#define SETTINGSEXPORTMENU_H_

#include "Menu.h"
#include "WidgetImageButton.h"

/// Меню "импорт и экспорт данных".
class SettingsExportMenu: public Menu {

public:

	/// Конструктор.
	SettingsExportMenu();

	/// Деструктор.
	virtual ~SettingsExportMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	std::shared_ptr<WidgetImageButton> m_import;
	std::shared_ptr<WidgetImageButton> m_export;

	void doExport();

};

#endif /* SETTINGSEXPORTMENU_H_ */
