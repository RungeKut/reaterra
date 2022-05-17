#include "Widget.h"

Widget::Widget(int x, int y, State active)
:	m_kbd_focus {false},
	m_x {x}, m_y {y}, m_height {0} , m_width {0}, m_state {active},
	m_parent {nullptr},
	m_visible {true}
{
}

Widget::~Widget() {
}

int Widget::getX() const {
	return m_x;
}

Widget *Widget::setX(int x) {
	this->m_x = x;

	return this;
}

int Widget::getY() const {
	return m_y;
}

Widget *Widget::setY(int y) {
	this->m_y = y;

	return this;
}

Widget::State Widget::getState() const {
	return m_state;
}

Widget *Widget::setState(State state) {
	m_state = state;
//	LD("Set state " + (m_state == Widget::State::Disabled ? "disabled" : "enabled"));
	return this;
}

int Widget::getHeight() const {
	return m_height;
}

void Widget::setHeight(int height) {
	m_height = height;
}

int Widget::getWidth() const {
	return m_width;
}

void Widget::setWidth(int wide) {
	m_width = wide;
}

bool Widget::isSelected(int x, int y) {
	bool ret = false;


	if (getWidth() != 0 && x > getX() && x < getX() + getWidth() && y > getY() && y < getY() + getHeight()) {
		ret = true;
	}

	return ret;
}

Menu* Widget::getParent() const {
	return m_parent;
}

void Widget::setParent(Menu* parent) {
	m_parent = parent;
}

std::string Widget::getStateName(const State state) {
	switch (state) {
	case State::Active:
		return "Active";
	case State::Disabled:
		return "Disabled";
	case State::Passive:
		return "Passive";
	}

	return "Unknown";
}

bool Widget::isVisible() const {
	return m_visible;
}

Widget *Widget::setVisible(bool visible) {
	m_visible = visible;
	return this;
}
