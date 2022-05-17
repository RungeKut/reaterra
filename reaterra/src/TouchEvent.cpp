#include "TouchEvent.h"

TouchEvent::TouchEvent(TouchType type, int x, int y)
:	Event(Event::EventType::Touch),
	m_type {type},
	m_x {x},
	m_y {y}
{
}

TouchEvent::~TouchEvent() {
}

TouchEvent::TouchType TouchEvent::getType() const {
	return m_type;
}

int TouchEvent::getX() const {
	return m_x;
}

int TouchEvent::getY() const {
	return m_y;
}
