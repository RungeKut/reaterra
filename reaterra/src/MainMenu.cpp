#include "MainMenu.h"

#include "Context.h"
#include "MenuEvent.h"
#include "ModalAuthAdmin.h"
#include "PatientListMenu.h"
#include "ProceduresMenu.h"
#include "RehabMenu.h"
#include "SettingsMenu.h"
#include "WidgetImageButton.h"
#include "Version.h"


MainMenu::MainMenu()
:	Menu {0, 0, "main_menu.png"}
{
	LD("Ctor");
	setType("MAIN");

	if (ctx.m_first_run) {
		ctx.m_first_run = false;
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
		ctx.m_treadmill.setAngle(0);
		ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
		ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
	}

	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.font_style = FontStyle::Normal;
	style.font_size = 19;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Center;
	m_back_btn->setState(Widget::State::Disabled);

	style.halign = Align::Right;
	style.font_size = 24;
	char buff[10];
	sprintf(buff, "%d.%d", VERSION / 100, VERSION % 100);
	addWidget(std::make_shared<WidgetTextField>(568, 0, 700, 47, style))->setText(M("Версия ПО") + ": " + buff);

	style.halign = Align::Center;
	style.font_size = 30;
	style.text_color_disabled = 0x306396;
	addWidget(std::make_shared<WidgetTextField>(0, 440, 1280, 80, style))->setText(M("БЕГОВАЯ ДОРОЖКА С БИОЛОГИЧЕСКОЙ ОБРАТНОЙ СВЯЗЬЮ"));

	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Right;
	style.lines = 6;
	style.font_size = 30;
	style.rect_color_active = to_base(Colors::None);;
	addWidget(std::make_shared<WidgetImageButton>(0, 550, "main_menu_patients.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(127, 550, 269, 121, style, M("ПАЦИЕНТЫ"))));
	addWidget(std::make_shared<WidgetImageButton>(427, 550, "main_menu_procedures.png", "", "",
			[](WidgetImageButton::Event e) {
//				if (ctx.m_settings.getParamString("treadmill_type") == "10") {
					return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());
//				} else {
//					return std::make_unique<MenuEvent>(std::make_unique<ProceduresMenu>());
//				}
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(555, 550, 269, 121, style, M("ПРОЦЕДУРЫ"))));
	addWidget(std::make_shared<WidgetImageButton>(855, 550, "main_menu_settings.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_active_widget = nullptr;
				m_modal = std::make_unique<ModalAuthAdmin>();
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(982, 550, 269, 121, style, M("НАСТРОЙКИ"))));


}

MainMenu::~MainMenu() {
	LD("Dtor");
}

Menu* MainMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	if (m_modal && event->getType() == Event::EventType::ModalYes && m_modal->getType() == "AUAD") {
		m_modal.reset();
		ctx.m_admin = event->getExtra();
		ret = new SettingsMenu(ctx.m_admin);
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	} else if (m_modal && event->getType() == Event::EventType::ModalCancel && m_modal->getType() == "AUAD") {
		ctx.m_admin = "";
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}
