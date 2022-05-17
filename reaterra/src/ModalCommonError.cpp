#include "ModalCommonError.h"

#include "Context.h"
#include "Menu.h"
#include "WidgetText.h"

ModalCommonError::ModalCommonError(std::string title, std::string message, std::string type)
:	Menu {369, 262, "modal_common_error.png", { //315
		std::make_shared<WidgetText>(315, 56, title, Align::Center, 24, static_cast<uint32_t>(Colors::BlueOnYellow), static_cast<uint32_t>(Colors::BlueOnYellow),
				std::function<std::string(void)>{}, FontStyle::Bold, 422, 3),
		std::make_shared<WidgetText>(271, 137, message, Align::Center, 24, static_cast<uint32_t>(Colors::Black), static_cast<uint32_t>(Colors::Black),
				std::function<std::string(void)>{}, FontStyle::Bold, 511, 3)
	}}
{
	LD("Ctor");
	m_check_events = false;
	ctx.m_sound.play(SoundInterface::Sound::Alarm);
	setType(type);
}

ModalCommonError::~ModalCommonError() {
	LD("Dtor");
}

