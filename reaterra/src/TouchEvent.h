#ifndef TOUCHEVENT_H_
#define TOUCHEVENT_H_

#include "Event.h"


/// Событие сенсорной панели.
class TouchEvent: public Event {

public:

	/// Тип события.
	enum class TouchType {
		Touch,		///< Прикосновение.
		Untouch,	///< Отрыв.
		Move,		///< Перемещение.
		Clear		///< Очистка.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param type тип события.
	 * @param x координата x.
	 * @param y координата y.
	 */
	TouchEvent(TouchType type, int x, int y);

	/// Деструктор.
	virtual ~TouchEvent();

	/*!
	 * @brief Получение типа события.
	 *
	 * @return тип события.
	 */
	TouchType getType() const;

	/*!
	 * @brief Получение координаты x.
	 *
	 * @return координата x.
	 */
	int getX() const;

	/*!
	 * @brief Получение координаты y.
	 *
	 * @return координата y.
	 */
	int getY() const;


private:

	TouchType m_type;
	int m_x;
	int m_y;

};

#endif /* TOUCHEVENT_H_ */
