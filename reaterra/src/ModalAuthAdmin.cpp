#include "ModalAuthAdmin.h"

#include "Context.h"
#include "ModalKeyboardAlpha.h"
#include "Utils.h"

ModalAuthAdmin::ModalAuthAdmin()
: Menu {391, 79, "modal_auth_admin.png"},
	m_admin_selected {false}
{
	LD("Ctor");
	setType("AUAD");
	WTFStyle style {default_style};

	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(160, 393, "modal_close_active.png", "", "button_close_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(160 + 9, 393 + 8, 122, 40, style, M("ЗАКРЫТЬ"))));


	style.font_style = FontStyle::Bold;
	style.font_size = 24;
	style.halign = Align::Left;
	style.text_color_passive = 0x434242;
	style.text_color_active = 0x003264;
	m_admin = addWidget(std::make_shared<WidgetImageButton>(37, 193, "modal_auth_admin_adm_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_doc->clearTrigger();
				m_admin_selected = true;
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(46, 192, 414, 45, style, M("Администратор"))));


	m_doc = addWidget(std::make_shared<WidgetImageButton>(37, 148, "modal_auth_admin_adm_active.png", "", "",
			[this](WidgetImageButton::Event e) {

				m_admin->clearTrigger();
				m_admin_selected = false;
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(46, 147, 414, 45, style, M("Врач"))));



	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(83, 114, 368, 34, style))->setText(M("ПОЛЬЗОВАТЕЛЬ") + ":");
	addWidget(std::make_shared<WidgetTextField>(93, 254, 368, 34, style))->setText(M("ПАРОЛЬ") + ":");

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(84, 15, 398, 88, style))->setText(M("ВЫБЕРИТЕ ПОЛЬЗОВАТЕЛЯ\\И ВВЕДИТЕ ПАРОЛЬ"));

	//TODO: Use WidgetTextField
	m_pwd = addWidget(std::make_shared<WidgetImageButton>(36, 289, "modal_auth_admin_pwd_active.png", "", "",
			[this](WidgetImageButton::Event e) {

				m_pwd_wrong_text->setText("");
				m_pwd_value = Utils::utf8_to_wstring("");
				m_pwd_text->setText("");
				m_modal = std::unique_ptr<Menu>(new ModalKeyboardAlpha());
				return nullptr;
		}, WidgetImageButton::Type::Trigger));

	m_pwd_text = addWidget(std::make_shared<WidgetText>(46, 325, "", Align::Left, 30, 0x1A1A1A, 0x1A1A1A));

	m_pwd_wrong_text = addWidget(std::make_shared<WidgetText>(93, 378, "", Align::Left, 30, 0xED7028, 0xED7028));

	if (m_admin_selected) {
		m_admin->setTrigger();
	} else {
		m_doc->setTrigger();
	}
}


ModalAuthAdmin::~ModalAuthAdmin() {
	LD("Dtor");
}


Menu* ModalAuthAdmin::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	std::string user;

	if (event->getType() == Event::EventType::KbdEvent) {
		auto tev =  dynamic_unique_cast<KbdEvent>(std::move(event));
		if (tev) {
			std::string s = tev->getKey();
//			LD("Event is     -       " + s);
			if (s == M("Отмена")) {
				m_pwd_value = Utils::utf8_to_wstring("");
				m_modal.reset();
				m_pwd->clearTrigger();
			} else if (s == M("Ввод")) {
				bool pwd_ok = false;
				std::string pwd = Utils::wstring_to_utf8(m_pwd_value);
				if (m_admin_selected) {
					if (pwd == ctx.m_settings.getParamString("admin_pwd")) {
						pwd_ok = true;
						user = "admin";
					}
				} else {
					if (pwd == ctx.m_db.getParam("ПарольВрача")) {
						pwd_ok = true;
						user = "doc";
					}
				}

				if (pwd_ok) {
					m_modal.reset();
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes, user));
					return nullptr;
				} else {
					m_pwd_wrong_text->setText(M("НЕВЕРНЫЙ ПАРОЛЬ"));
					m_modal.reset();
					m_pwd->clearTrigger();
					ctx.m_sound.play(SoundInterface::Sound::Alarm);
				}
			} else if (s == "←") {
				if (m_pwd_value.size() > 0) {
					m_pwd_value.pop_back();
				}
			} else if (!s.empty() && m_pwd_value.size() < 20) {
				m_pwd_value += Utils::utf8_to_wstring(s);
			}

			std::string str;
			for (unsigned i = 0; i < m_pwd_value.size(); ++i) {
				str += "\u2022";
			}
			m_pwd_text->setText(str);
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
		return nullptr;
	}

	ret = Menu::processEvent(std::move(event));
	return ret;
}
