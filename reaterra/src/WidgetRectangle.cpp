#include "WidgetRectangle.h"

#include "Context.h"

WidgetRectangle::WidgetRectangle(int x, int y, int width, int height, uint32_t color, bool visible)
:	Widget(x, y, State::Disabled),
	m_color {color}
{
	setWidth(width);
	setHeight(height);
	setVisible(visible);
}

WidgetRectangle::~WidgetRectangle() {
}

void WidgetRectangle::show() {
	if (isVisible()) {
		ctx.m_graphic.drawRectangle(getX(), getY(), getWidth(), getHeight(), m_color);
	}
}

void WidgetRectangle::setColor(uint32_t color) {
	m_color = color;
}

