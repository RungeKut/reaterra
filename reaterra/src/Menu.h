#ifndef MENU_H_
#define MENU_H_

#include <memory>
#include <vector>

#include "Locale.h"
#include "Timer.h"
#include "TouchEvent.h"
#include "WidgetBreadCrumbs.h"

/// Базовый класс меню.
class Menu {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param background фоновая картинка.
	 * @param widgets набор виджетов.
	 * @param menu_type тип (название) меню.
	 */
	Menu(int x = 0, int y = 0, std::string background = "", WidgetSPVec &&widgets = {}, std::string menu_type = "");

	// Деструктор.
	virtual ~Menu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	virtual Menu *processEvent(std::unique_ptr<Event> event);

	/*!
	 * @brief Отрисовка изображения.
	 *
	 * @param final true: финальная отрисовка на экране, false: промежуточная отрисовка в буфере.
	 */
	virtual void show(bool final = true);

	/*!
	 * @brief Поиск виджета по координатам.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @return ссылка на виджет.
	 */
	WidgetSP getSelected(int x, int y);

	/*!
	 * @brief Получение координаты x меню.
	 *
	 * @return координата x.
	 */
	int getX() const;

	/*!
	 * @brief Установка координаты x меню.
	 *
	 * @param x координата x.
	 */
	void setX(int x);


	/*!
	 * @brief Получение координаты y меню.
	 *
	 * @return координата y.
	 */
	int getY() const;


	/*!
	 * @brief Установка координаты y меню.
	 *
	 * @param y координата y.
	 */
	void setY(int y);

	/*!
	 * @brief Получение типа меню.
	 *
	 * @return тип меню.
	 */
	const std::string& getType() const;

	/*!
	 * @brief Установка типа меню.
	 *
	 * @param type тип меню.
	 */
	void setType(const std::string& type);

	//TODO: Перенести m_modal в приват или протектед
	/// Указатель на модальное меню
	std::unique_ptr<Menu> m_modal;

	/*!
	 * @brief Получение ширины меню.
	 *
	 * @return ширина меню.
	 */
	int getWidth() const;

	/*!
	 * @brief Получение высоты меню.
	 *
	 * @return высота меню.
	 */
	int getHeight() const;

	/*!
	 * @brief Установка текущего пациента.
	 *
	 * @param name имя пациента.
	 * @param id идентификатор.
	 * @param weight вес.
	 */
	void setPatient(std::string name, int id, float weight, int profile_id);

	/*!
	 * @brief Добавление виджета.
	 *
	 * @param widget ссылка на виджет.
	 * @return ссылка на добавленный виджет.
	 */
	template <class T> std::shared_ptr<T> addWidget(std::shared_ptr<T> widget) {
		static_assert(std::is_base_of<Widget, T>::value, "Type must derive from Widget");
		widget->setParent(this);
		m_widgets.push_back(widget);
		return widget;
	}


	/// Обработка события нажатия на сенсорной панели.
	Menu* touchProcess(std::unique_ptr<Event> event);

protected:

	enum class EventType {
		Stop,
		Pause,
		None
	};

	int m_background_id;
	WidgetSPVec m_widgets;
	WidgetSP m_active_widget;
	int m_x;
	int m_y;
	std::string m_type;
	bool m_check_events;
	int m_width;
	int m_height;
	EventType m_event;
	WidgetImageButtonSP m_patients_btn;
	WidgetImageButtonSP m_hr_btn;
	WidgetImageButtonSP m_back_btn;
	WidgetImageButtonSP m_unl_batt;
	std::function<std::unique_ptr<::Event>(WidgetImageButton::Event)> m_on_back;
	bool m_prev_unl_warn;
	EventType checkEvents();
	void addBreadCrumps(std::vector<WBCElement> &&bread_crumps);
	void addOnBackAction(std::function<std::unique_ptr<::Event> (WidgetImageButton::Event)> on_back);


private:

	Timer m_clock_timer;
	int m_sas_service_menu_counter = 5;

};

#endif /* MENU_H_ */
