#include "SettingsPasswordMenu.h"

#include "Context.h"
#include "KbdEvent.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "ModalKeyboardAlpha.h"
#include "SettingsMenu.h"
#include "Utils.h"
#include "WidgetBreadCrumbs.h"

SettingsPasswordMenu::SettingsPasswordMenu()
:	Menu {0, 0, "settings_pwd.png", {}}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("смена пароля врача"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	style.halign = Align::Center;
	style.lines = 6;
	style.hpad = 0;
	style.font_size = 22;
	style.rect_color_active = to_base(Colors::None);;

	WidgetTextFieldSP lbl = std::make_shared<WidgetTextField>(460, 466, 274, 78, style, M("СОХРАНИТЬ ПАРОЛЬ"));
	m_pwd = std::make_shared<WidgetImageButton>(428, 346, "settings_pwd_entry_active.png", "", "",
			[this, lbl](WidgetImageButton::Event e) {

				m_pwd_value = Utils::utf8_to_wstring("");
				m_pwd_text->setText("");
				m_save_button->setState(Widget::State::Passive);
				lbl->setText(M("СОХРАНИТЬ ПАРОЛЬ"));
				m_modal = std::unique_ptr<Menu>(new ModalKeyboardAlpha());
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_pwd);

	m_save_button = std::make_shared<WidgetImageButton>(459, 465, "settings_pwd_save_active.png", "settings_pwd_complete_active.png", "",
			[this, lbl](WidgetImageButton::Event e) {
				std::string pwd = Utils::allTrim(Utils::wstring_to_utf8(m_pwd_value));

				if (pwd.empty()) {
					using Flags = ModalCommonDialog::Flags;
					m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
							M("ПАРОЛЬ НЕ МОЖЕТ БЫТЬ ПУСТЫМ"), "SVER", Flags::YellowTitle | Flags::CloseButton,
							"icon_save_cross_yellow.png"));
				} else {
					ctx.m_db.setParam("ПарольВрача", pwd);
					m_pwd_value = Utils::utf8_to_wstring("");
					m_pwd_text->setText("");
					m_save_button->setState(Widget::State::Disabled);
					lbl->setText(M("ГОТОВО"));
				}
				return nullptr;
		}, WidgetImageButton::Type::Clicker, lbl);
	addWidget(m_save_button);

	style.text_color_disabled = 0x003264;
	style.font_size = 26;
	addWidget(std::make_shared<WidgetTextField>(457, 248, 431, 67, style, M("НОВЫЙ ПАРОЛЬ ВРАЧА:")));

	m_pwd_text = std::make_shared<WidgetText>(435, 381, "", Align::Left, 30, 0x1A1A1A, 0x1A1A1A);
	addWidget(m_pwd_text);

	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Left;

	addWidget(std::make_shared<WidgetTextField>(467, 100, 623, 67, style, M("ОТКЛЮЧИТЬ ПАРОЛЬ ВРАЧА")));
	std::string check = ctx.m_notuse_doc_password ? "rehab_procedures_hr_selector_active.png" : "rehab_procedures_hr_selector_passive.png";
	m_doc_pass = addWidget(std::make_shared<WidgetImageButton>(384, 102, check, "", check,
			[this](WidgetImageButton::Event e) {
				ctx.m_notuse_doc_password = !ctx.m_notuse_doc_password;
				if (ctx.m_notuse_doc_password) {
					m_doc_pass->setPassive("rehab_procedures_hr_selector_active.png");
					m_doc_pass->setActive("rehab_procedures_hr_selector_active.png");
				} else {
					m_doc_pass->setPassive("rehab_procedures_hr_selector_passive.png");
					m_doc_pass->setActive("rehab_procedures_hr_selector_passive.png");
				}
				ctx.m_db.setParam("ОтключитьПарольВрача", ctx.m_notuse_doc_password ? "1" : "0");
				return nullptr;
		}));
}

SettingsPasswordMenu::~SettingsPasswordMenu() {
	LD("Dtor");
}

Menu* SettingsPasswordMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

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
				std::string pwd = Utils::wstring_to_utf8(m_pwd_value);
				m_modal.reset();
				m_pwd->clearTrigger();
			} else if (s == "←") {
				if (m_pwd_value.size() > 0) {
					m_pwd_value.pop_back();
				}
			} else if (!s.empty() && m_pwd_value.size() < 20) {
				m_pwd_value += Utils::utf8_to_wstring(s);
			}
			m_pwd_text->setText(Utils::wstring_to_utf8(m_pwd_value));
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
		return nullptr;
	}


	if (m_modal && m_modal->getType() == "SVER" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	} else {
		if (m_modal && event->getType() == Event::EventType::ModalYes) {
				m_pwd_value = Utils::utf8_to_wstring("");
				m_pwd_text->setText(Utils::wstring_to_utf8(m_pwd_value));
				m_pwd->clearTrigger();
		}
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}
