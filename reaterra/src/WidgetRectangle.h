#ifndef WIDGETRECTANGLE_H_
#define WIDGETRECTANGLE_H_

#include "Widget.h"

/// Виджет отображения прямоугольника.
class WidgetRectangle: public Widget {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param width ширина.
	 * @param height высота.
	 * @param color цвет.
	 * @param visible видимость.
	 */
	WidgetRectangle(int x, int y, int width, int height, uint32_t color = 0x808080, bool visible = false);

	/// Деструктор.
	virtual ~WidgetRectangle();

	/// Отрисовка изображения.
	void show() override;

	/*!
	 * @brief Установка цвета.
	 *
	 * @param color цвет.
	 */
	void setColor(uint32_t color);

private:

	uint32_t m_color;

};

#endif /* WIDGETRECTANGLE_H_ */
