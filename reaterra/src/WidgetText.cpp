#include "WidgetText.h"

#include "Context.h"

WidgetText::WidgetText(int x, int y, std::string text, Align align,
		int size, uint32_t color, uint32_t color_disabled, std::function<std::string(void)> on_update,
		FontStyle font_style, int width, int lines, Align v_align, int dy)
: Widget(x, y, State::Disabled),
  m_text {text},
  m_align {align},
  m_font_size {size},
  m_color {color},
  m_color_disabled {color_disabled},
  m_on_update {on_update},
  m_font_style {font_style},
  m_lines {lines},
  m_v_align {v_align},
  m_dy {static_cast<int>(dy ? dy : 1.2 * size)}
{
//	LD("Ctor");
	setWidth(width);
}

WidgetText::~WidgetText() {
}

Align WidgetText::getAlign() const {
	return m_align;
}

void WidgetText::setAlign(Align align) {
	m_align = align;
}

uint32_t WidgetText::getColor() const {
	return m_color;
}

void WidgetText::setColor(uint32_t color) {
	m_color = color;
}

int WidgetText::getFontSize() const {
	return m_font_size;
}

void WidgetText::setFontSize(int fontSize) {
	m_font_size = fontSize;
}

const std::string& WidgetText::getText() const {
	return m_text;
}

void WidgetText::show() {
	if (m_on_update) {
		m_text = m_on_update();
	}

	if (m_text.empty()) {
		return;
	}

	int x = 0;
	int y = 0;

	if (getParent()) {
		x = getParent()->getX();
		y = getParent()->getY();
	}

	uint32_t c;
	if (getState() == State::Disabled) {
		c = getColorDisabled();
	} else {
		c = getColor();
	}

	if (getWidth()) {
		ctx.m_graphic.drawMultiText(m_text, x + getX(), y + getY(), getWidth(), m_lines, m_dy, getFontSize(), getAlign(), m_v_align, m_font_style, c);
	} else {
		ctx.m_graphic.drawText(m_text.c_str(), x + getX(), y + getY(), getFontSize(),
				RGB2R(c), RGB2G(c), RGB2B(c), getAlign(), m_font_style);
	}
}

uint32_t WidgetText::getColorDisabled() const {
	return m_color_disabled;
}

void WidgetText::setText(const std::string& text) {
	m_text = text;
}

//void WidgetText::setTextFieldParams(int width, int height, std::string active, std::string disabled, FieldType field_type) {
//	m_field_type = field_type;
//	if (m_back_button) {
//		m_back_button->setActive(active);
//		m_back_button->setDisabled(disabled);
//	} else {
//		m_back_button = std::make_shared<WidgetImageButton>();
//	}
//}
