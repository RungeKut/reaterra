#include "Event.h"

Event::Event(EventType type, std::string extra)
:	m_type {type},
	m_extra {extra}
{
}

Event::~Event() {
}

Event::EventType Event::getType() const {
	return m_type;
}

std::string Event::getName() {
	switch (m_type) {
	case EventType::Quit:
		return "Quit";
	case EventType::Touch:
		return "Touch";
	case EventType::Timer:
		return "Timer";
	case EventType::TreadmillConnected:
		return "TreadmillConnected";
	case EventType::TreadmillReady:
		return "TreadmillReady";
	case EventType::TreadmillDisconnected:
		return "TreadmillDisconnected";
	case EventType::Refresh:
		return "Refresh";
	case EventType::Menu:
		return "Menu";
	case EventType::ModalYes:
		return "ModalYes";
	case EventType::ModalNo:
		return "ModalNo";
	case EventType::ModalCancel:
		return "ModalCancel";
	case EventType::Zastup:
		return "Zastup";
	case EventType::EmergencyButton:
		return "EmergencyButton";
	case EventType::KbdEvent:
		return "KbdEvent";
	case EventType::HrMonitorEvent:
		return "HrMonitorEvent";
	case EventType::Shutdown:
		return "Shutdown";
	case EventType::Restart:
		return "Restart";
	}

	return "UNKNOWN";
}

std::string Event::getExtra() {
	return m_extra;
}
