#ifndef WIDGET_H_
#define WIDGET_H_

#include "GraphicEngine.h"
#include "Logger.h"
#include "Event.h"


enum class Colors : uint32_t {
	Black = 0,
	White = 0xFFFFFF,
	BlueOnYellow = 0x003264,
	None = 0xFFFFFFFF
};


class Menu;
/// Базовый класс виджетов.
class Widget {

public:

	/// Состояние виджета.
	enum class State {
		Active,		///< Активный.
		Passive,	///< Пассивный.
		Disabled	///< Заблокированный.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param active состояние.
	 */
	Widget(int x = 0, int y = 0, State active = State::Passive);

	/// Деструктор.
	virtual ~Widget();

	/*!
	 * @brief Получение координаты x.
	 *
	 * @return координата x.
	 */
	int getX() const;

	/*!
	 * @brief Установка координаты x.
	 *
	 * @param x
	 * @return ссылка на виджет.
	 */
	Widget *setX(int x);

	/*!
	 * @brief Получение координаты y.
	 *
	 * @return координата y.
	 */
	int getY() const;

	/*!
	 * @brief Установка координаты y.
	 *
	 * @param y координата y.
	 * @return ссылка на виджет.
	 */
	Widget *setY(int y);

	/*!
	 * @brief Получение состояния.
	 *
	 * @return состояние.
	 */
	State getState() const;

	/*!
	 * @brief Установка состояния.
	 *
	 * @param state состояние.
	 * @return ссылка на виджет.
	 */
	virtual Widget *setState(State state);

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	virtual Menu *processEvent(Event *event) {return nullptr;};

	/// Отрисовка изображения.
	virtual void show() {}

	/*!
	 * @brief Получение высоты виджета.
	 *
	 * @return высота виджета.
	 */
	int getHeight() const;

	/*!
	 * @brief Установка высоты виджета.
	 *
	 * @param height высота виджета.
	 */
	void setHeight(int height);

	/*!
	 * @brief Получение ширины виджета.
	 *
	 * @return ширина виджета.
	 */
	int getWidth() const;

	/*!
	 * Установка ширины виджета.
	 *
	 * @param wide ширина виджета.
	 */
	void setWidth(int wide);

	/*!
	 * @brief Определение нажатия.
	 *
	 * @param x координата x касания.
	 * @param y координата y касания.
	 * @return true: есть нажатие, false: нет нажатия.
	 */
	bool isSelected(int x, int y);

	/*!
	 * @brief Получение родительского виджета.
	 *
	 * @return ссылка на родительский виджет.
	 */
	Menu* getParent() const;

	/*!
	 * @brief Установка родительского виджета.
	 *
	 * @param parent ссылка на родительский виджет.
	 */
	void setParent(Menu* parent);

	/*!
	 * @brief Получение названия состояния.
	 *
	 * @param state состояние.
	 * @return название состояния.
	 */
	static std::string getStateName(const State state);

	/*!
	 * @brief Получение видимости.
	 *
	 * @return true: виджет отображается, false: виджет не отображается.
	 */
	bool isVisible() const;

	/*!
	 * @brief Установка видимости.
	 *
	 * @param visible true: виджет отображается, false: виджет не отображается.
	 * @return ссылка на виджет.
	 */
	Widget *setVisible(bool visible);

	/// true: виджет имеет фокус клавиатуры, false: виджет не имеет фокуса клавиатуры.
	bool m_kbd_focus;

private:
	int m_x;
	int m_y;
	int m_height;
	int m_width;
	State m_state;
	Menu *m_parent;
	bool m_visible;

};

typedef std::shared_ptr<Widget> WidgetSP;
typedef std::vector<WidgetSP> WidgetSPVec;


#endif /* WIDGET_H_ */
