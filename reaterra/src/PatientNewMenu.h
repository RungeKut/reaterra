#ifndef PATIENTNEWMENU_H_
#define PATIENTNEWMENU_H_

#include "Menu.h"

#include "DbConnector.h"
#include "MenuEvent.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetTextField.h"

/// Меню "создания/редактирования учетной записи пациента"
class PatientNewMenu: public Menu {

public:

	/// Режим.
	enum class Mode {
		New,	///< Создание.
		Edit	///< Редактирование.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param id идентификатор пациента.
	 * @param mode режим.
	 */
	PatientNewMenu();

	/// Деструктор.
	virtual ~PatientNewMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

	/*!
	 * @brief Установка профиля пациента.
	 *
	 * @param profile_id идентификатор профиля.
	 * @param profile_name имя профиля.
	 */
	static void setProfile(std::string profile_id, std::string profile_name);

	/*!
	 * @brief Получение профиля пациента.
	 *
	 * @return имя профиля.
	 */
	static std::string getProfile();


private:

	DataSet m_patient;
	std::shared_ptr<WidgetImageButton> m_male;
	std::shared_ptr<WidgetImageButton> m_female;
	Mode m_mode;
	bool m_changed;
	std::string m_prev_fio;
	std::unique_ptr<Menu> m_next_menu;

	std::unique_ptr<MenuEvent> backAction(std::unique_ptr<Menu> menu);
	void saveState();
	void restoreState();

};

#endif /* PATIENTNEWMENU_H_ */
