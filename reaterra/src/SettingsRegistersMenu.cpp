#include "SettingsRegistersMenu.h"

#include "algorithm"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"
#include "SettingsExtraCalibActuatorMenu.h"
#include "SettingsExtraCalibDriveMenu.h"
#include "WidgetBreadCrumbs.h"

using Reg = TreadmillDriver::Reg;

SettingsRegistersMenu::SettingsRegistersMenu()
:	Menu {0, 0, "settings_registers_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_reg_prev {
			{Reg::PLC_STAT, 0}, {Reg::PLC_CS, 0}, {Reg::PLC_CD, 0}, {Reg::PLC_CTA, 0}, {Reg::PLC_CTD, 0}, {Reg::PLC_SD, 0},
			{Reg::PLC_ED, 0}, {Reg::PLC_CL, 0}, {Reg::PLC_AX, 0}, {Reg::PLC_ACX, 0}, {Reg::PLC_ACY_V0, 0}, {Reg::PLC_TYPE, 0},
			{Reg::PLC_VER, 0}, {Reg::PLC_CONF, 0}
		}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("системная информация"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	int line = 0;
	int line_width = 45;
	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.font_style = FontStyle::Normal;
	style.font_size = 25;
	style.text_color_disabled = 0x434242;
	style.halign = Align::Left;
	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_STAT);
			int bi = r & 0x1;
			int bc = (r >> 1) & 0x1;
			int bta = (r >> 2) & 0x1;
			int ffo = (r >> 3) & 0x1;
			int olp = (r >> 4) & 0x1;
			int ek = (r >> 5) & 0x1;
			int os = (r >> 6) & 0x1;
			int inv = (r >> 7) & 0x1;
			int sm = (r >> 8) & 0x1;
			char buff[128];
			sprintf(buff, "PLC_STAT(%2d) {%#06x} [BI:%d BC:%d BTA:%d FFO:%d OLP:%d EK:%d OS:%d INV:%d SM:%d] System status",
					to_base(TreadmillDriver::Reg::PLC_STAT), r, bi, bc, bta, ffo, olp, ek, os, inv, sm);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CS);
			int rca = r & 0x1;
			int dal = (r >> 1) & 0x1;
			int wm = (r >> 2) & 0x1;
			int fr = (r >> 3) & 0x1;
			char buff[128];
			sprintf(buff, "PLC_CS  (%2d) {%#06x} [RCA:%d DAL:%d WM:%d FR:%d] System control",
					to_base(TreadmillDriver::Reg::PLC_CS), r, rca, dal, wm, fr);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CD);
			int ds = r & 0x03FF;
			int dnf = (r >> 10) & 0x1;
			int rcwd = (r >> 11) & 0x1;
			int rcup = (r >> 12) & 0x1;
			int dfr = (r >> 13) & 0x1;
			int dsta_ = (r >> 14) & 0x3;
			char dsta[57] = "RESERVE";
			if (dsta_ == 0) {
				strcpy(dsta, "PAUSE");
			} else if (dsta_ == 1) {
				strcpy(dsta, "STOP");
			} else if (dsta_ == 2) {
				strcpy(dsta, "START");
			}
			char buff[256];
			sprintf(buff, "PLC_CD  (%2d) {%#06x} [DriveSpeed:%5.2f DNF:%d RCDW:%d RCUP:%d DFR:%d DSTA:%s] Control Drive",
					to_base(TreadmillDriver::Reg::PLC_CD), r, ds / 10., dnf, rcwd, rcup, dfr, dsta);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CTA);
			char buff[128];
			sprintf(buff, "PLC_CTA (%2d) {%#06x} [%6.1f] Control Time Acceleration",
					to_base(TreadmillDriver::Reg::PLC_CTA), r, r / 10.);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CTD);
			char buff[128];
			sprintf(buff, "PLC_CTD (%2d) {%#06x} [%6.1f] Control Time Deceleration",
					to_base(TreadmillDriver::Reg::PLC_CTD), r, r / 10.);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_SD);
			int ds = r & 0x03ff;
			int csta = (r >> 14) & 0x1;
			int cfr = (r >> 15) & 0x1;
			char buff[128];
			sprintf(buff, "PLC_SD  (%2d) {%#06x} [DriveSpeed:%5.2f CSTA:%d CFR:%d] Drive Status",
					to_base(TreadmillDriver::Reg::PLC_SD), r, ds / 10., csta, cfr);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_ED);
			char buff[128];
			sprintf(buff, "PLC_ED (%2d) {%#06x} [%3d] Drive errors",
					to_base(TreadmillDriver::Reg::PLC_ED), r, r);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t v = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CL);
			int an = v & 0x0FFF;
			int ad = (v >> 12) & 0x1;
			int pd = (v >> 13) & 0x1;
			int cdw = (v >> 14) & 0x1;
			int cup = (v >> 15) & 0x1;
			char buff[128];
			sprintf(buff, "PLC_CL  (%2d) {%#06x} [Angle:%5.2f AD:%d PD:%d CDW:%d CUP:%d] Control Lift",
					to_base(TreadmillDriver::Reg::PLC_CL), v, an / 100., ad, pd, cdw, cup);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_AX);
			char buff[128];
			sprintf(buff, "PLC_AX  (%2d) {%#06x} [%5.2f] Angle X",
					to_base(TreadmillDriver::Reg::PLC_AX), r, r / 100.);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	style.update_func = [this](WidgetTextField &w) {
			uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_ACX);
			char buff[128];
			sprintf(buff, "PLC_ACX  (%2d) {%#06x} [%5.2f] Calibrate Angle X",
					to_base(TreadmillDriver::Reg::PLC_ACX), r, static_cast<short>(r) / 100.);
			return std::string {buff};
		};
	addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

	if (ctx.m_treadmill.getControllerVersion() == 0) {
		style.update_func = [this](WidgetTextField &w) {
				uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_ACY_V0);
				char buff[128];
				sprintf(buff, "PLC_ACY  (%2d) {%#06x} [%5.2f] Calibrate Angle Y",
						to_base(TreadmillDriver::Reg::PLC_ACY_V0), r, static_cast<short>(r) / 100.);
				return std::string {buff};
			};
		addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));
	} else if (ctx.m_treadmill.getControllerVersion() > 0) {
		style.update_func = [this](WidgetTextField &w) {
				uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_TYPE);
				char buff[128];
				sprintf(buff, "PLC_TYPE  (%2d) {%#06x} [%5d] Treadmill type",
						to_base(TreadmillDriver::Reg::PLC_TYPE), r, static_cast<short>(r) & 0xFFF);
				return std::string {buff};
			};
		addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

		style.update_func = [this](WidgetTextField &w) {
				uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_CONF);
				char buff[128];
				sprintf(buff, "PLC_CONF  (%2d) {%#06x} [%5d] Treadmill configuration",
						to_base(TreadmillDriver::Reg::PLC_CONF), r, static_cast<short>(r));
				return std::string {buff};
			};
		addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));

		style.update_func = [this](WidgetTextField &w) {
				uint16_t r = ctx.m_treadmill.getRegisterState(TreadmillDriver::Reg::PLC_VER);
				char buff[128];
				sprintf(buff, "PLC_VER  (%2d) {%#06x} [%5d] Treadmill firmware version",
						to_base(TreadmillDriver::Reg::PLC_VER), r, static_cast<short>(r));
				return std::string {buff};
			};
		addWidget(std::make_shared<WidgetTextField>(10, 46 + line++ * line_width, 1280, 53, style));
	}

	style = default_style;
	style.font_size = 22;
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_passive = to_base(Colors::White);
	style.rect_color_active = to_base(Colors::None);
	style.text = M("КАЛИБРОВКА\\ПРИВОДА");
	addWidget(std::make_shared<WidgetImageButton>(997, 580, "settings_reg_active.png", "", "settings_reg_passive.png",
			[this](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsExtraCalibDriveMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(997, 580, 270, 62, style)));
	style.text = M("КАЛИБРОВКА\\ПОДЪЕМНИКА");
	addWidget(std::make_shared<WidgetImageButton>(997, 652, "settings_reg_active.png", "", "settings_reg_passive.png",
			[this](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsExtraCalibActuatorMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(997, 652, 270, 62, style)));

	style.text = M("ТЕСТИРОВАНИЕ\\АРС с прогоном");
	addWidget(std::make_shared<WidgetImageButton>(997, 500, "settings_reg_active.png", "", "settings_reg_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_menu_history.pop_back();
				ctx.m_menu_history.pop_back();
				ctx.m_run_sas_test = true;
				ctx.m_treadmill.restartSasTest();
				return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(997, 500, 270, 62, style)));


	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(384, 657, 402, 54, style, M("Язык интерфейса")));
	style.check_func = [this](std::string s, WidgetTextField &w) {
		if (!s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !(isalpha(c) && isupper(c)); }) == s.end()) {
			if (ctx.m_settings.getParamString("locale") != s) {
				ctx.m_settings.setParamString("locale", s);
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Restart));
			}
			return "";
		} else {
			w.setText(ctx.m_settings.getParamString("locale"));
			return "fail";
		}
	};

	style.text_max_size = 3;
	style.rect_color_active = 0x87CEEA;
	style.rect_color_passive = 0xCBCCCC;
	style.text_color_passive = to_base(Colors::Black);
	style.text_color_active = to_base(Colors::Black);
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(800, 657, 100, 54, style))
		->setText(ctx.m_settings.getParamString("locale"))
		.setState(Widget::State::Passive);


	m_timer.startTimerMillis(100);
}

SettingsRegistersMenu::~SettingsRegistersMenu() {
	LD("Dtor");
}

Menu* SettingsRegistersMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "run") {
		bool refresh {false};
		for (auto &kv : m_reg_prev) {
			uint16_t v = ctx.m_treadmill.getRegisterState(kv.first);
			if (v != kv.second) {
				m_reg_prev[kv.first] = v;
				refresh = true;
			}
		}
		if (refresh) {
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	}

	ret = Menu::processEvent(std::move(event));
	return ret;
}
