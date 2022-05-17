#include "KbdEvent.h"


KbdEvent::KbdEvent(std::string key)
:	Event {EventType::KbdEvent},
	m_key {key}
{
}


KbdEvent::~KbdEvent() {
}


const std::string& KbdEvent::getKey() const {
	return m_key;
}
