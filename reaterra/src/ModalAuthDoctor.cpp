#include "ModalAuthDoctor.h"

#include "Context.h"


ModalAuthDoctor::ModalAuthDoctor()
: Menu {391, 218, "rehab_user_protocols_auth_menu.png"}
{
	LD("Ctor");
	setType("AUDO");


	WTFStyle style {default_style};

	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(161, 269, "modal_close_active.png", "", "",
			[](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(170, 277, 122, 40, style, M("ЗАКРЫТЬ"))));

	style.text_color_disabled = to_base(Colors::White);
	style.font_size = 24;
	addWidget(std::make_shared<WidgetTextField>(97, 15, 385, 88, style))->setText(M("ВВЕДИТЕ ПАРОЛЬ ВРАЧА"));
	style.text_color_disabled = 0x434242;
	style.halign = Align::Left;
	style.font_style = FontStyle::Normal;
	addWidget(std::make_shared<WidgetTextField>(92, 129, 260, 32, style))->setText(M("ПАРОЛЬ") + ":");

	style = default_style;
	style.password_char = "\u2022";
	style.text_max_size = 20;
	style.halign = Align::Left;
	style.check_func = [this](std::string s, WidgetTextField &w) {
		LD("Got: " + s);
		if (s == ctx.m_db.getParam("ПарольВрача") || s == ctx.m_settings.getParamString("admin_pwd")) {
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
		} else {
			m_pwd_wrong->setText(M("НЕВЕРНЫЙ ПАРОЛЬ"));
			m_pwd_entry->setText("");
			ctx.m_sound.play(SoundInterface::Sound::Alarm);
		}
		return "";
	};
	m_pwd_entry = addWidget(std::make_shared<WidgetTextField>(36, 164, 424, 49, style));
	m_pwd_entry->setState(Widget::State::Passive);

	style = default_style;
	style.text_color_disabled = 0xED7028;
	m_pwd_wrong = addWidget(std::make_shared<WidgetTextField>(37, 219, 421, 41, style));
}

ModalAuthDoctor::~ModalAuthDoctor() {
	LD("Dtor");
}
