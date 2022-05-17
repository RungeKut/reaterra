#include "SettingsAccelMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"
#include "WidgetBreadCrumbs.h"

SettingsAccelMenu::SettingsAccelMenu()
:	Menu {0, 0, "settings_accel_1_active.png"},
	m_accel_prev {ctx.m_accel},
	m_decel_prev {ctx.m_decel}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("задание ускорения и торможения"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	std::string axis_file = "settings_accel_axis_10.png";
	if (ctx.m_treadmill.getMaxForwardSpeed() > 220) {
		axis_file = "settings_accel_axis_30.png";
	} else if (ctx.m_treadmill.getMaxForwardSpeed() > 100) {
		axis_file = "settings_accel_axis_20.png";
	}

	addWidget(std::make_shared<WidgetImageButton>(28, 277, "", axis_file));
	m_widgets.back()->setState(Widget::State::Disabled);

	addWidget(std::make_shared<WidgetImageButton>(644, 277, "", axis_file));
	m_widgets.back()->setState(Widget::State::Disabled);

	m_accel_chart = std::make_shared<WidgetImageButton>(59, 277, "");
	m_accel_chart->setState(Widget::State::Disabled);
	addWidget(m_accel_chart);
	m_decel_chart = std::make_shared<WidgetImageButton>(675, 277, "");
	m_decel_chart->setState(Widget::State::Disabled);
	addWidget(m_decel_chart);

	m_accel_1 = std::make_shared<WidgetImageButton>(60, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 1;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_1);

	m_accel_2 = std::make_shared<WidgetImageButton>(138, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 2;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_2);

	m_accel_3 = std::make_shared<WidgetImageButton>(216, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 3;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_3);

	m_accel_4 = std::make_shared<WidgetImageButton>(294, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 4;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_4);

	m_accel_5 = std::make_shared<WidgetImageButton>(372, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 5;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_5);

	m_accel_6 = std::make_shared<WidgetImageButton>(450, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 6;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_6);

	m_accel_7 = std::make_shared<WidgetImageButton>(528, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_accel = 7;
				updateAccel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_accel_7);


	m_decel_1 = std::make_shared<WidgetImageButton>(676, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 1;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_1);

	m_decel_2 = std::make_shared<WidgetImageButton>(754, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 2;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_2);

	m_decel_3 = std::make_shared<WidgetImageButton>(832, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 3;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_3);

	m_decel_4 = std::make_shared<WidgetImageButton>(910, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 4;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_4);

	m_decel_5 = std::make_shared<WidgetImageButton>(988, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 5;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_5);

	m_decel_6 = std::make_shared<WidgetImageButton>(1066, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 6;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_6);

	m_decel_7 = std::make_shared<WidgetImageButton>(1144, 106, "settings_accel_selector_active.png", "", "",
			[&](WidgetImageButton::Event e) {
				ctx.m_decel = 7;
				updateDecel();
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_decel_7);


	WTFStyle style {default_style};

	style.halign = Align::Left;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(60, 153, 259, 66, style, M("ВРЕМЯ РАЗГОНА\\от 0 до ")
			+ std::to_string(ctx.m_treadmill.getMaxForwardSpeed() / 10) + " " + M("км/ч")));
	addWidget(std::make_shared<WidgetTextField>(454, 184, 125, 31, style, M("сек")));
	addWidget(std::make_shared<WidgetTextField>(60, 219, 259, 43, style, M("УСКОРЕНИЕ")));
	addWidget(std::make_shared<WidgetTextField>(454, 222, 125, 31, style, M("м/сек2")));

	addWidget(std::make_shared<WidgetTextField>(676, 153, 291, 66, style, M("ВРЕМЯ ТОРМОЖЕНИЯ\\от ")
			+ std::to_string(ctx.m_treadmill.getMaxForwardSpeed() / 10) + M(" до 0 ") + M("км/ч")));
	addWidget(std::make_shared<WidgetTextField>(1103, 184, 125, 31, style, M("сек")));
	addWidget(std::make_shared<WidgetTextField>(676, 219, 291, 43, style, M("ТОРМОЖЕНИЕ")));
	addWidget(std::make_shared<WidgetTextField>(1103, 222, 125, 31, style, M("м/сек2")));

	addWidget(std::make_shared<WidgetTextField>(59, 46, 546, 47, style, M("УРОВЕНЬ УСКОРЕНИЯ")));
	addWidget(std::make_shared<WidgetTextField>(676, 46, 546, 47, style, M("УРОВЕНЬ ТОРМОЖЕНИЯ")));

	style.halign = Align::Center;
	style.font_size = 20;
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	addWidget(std::make_shared<WidgetTextField>(192, 590, 321, 35, style, M("УРОВЕНЬ УСКОРЕНИЯ")));
	addWidget(std::make_shared<WidgetTextField>(802, 343, 321, 35, style, M("УРОВЕНЬ ТОРМОЖЕНИЯ")));

	style.halign = Align::Center;
	style.font_size = 20;
	style.text_color_disabled = to_base(Colors::Black);
	addWidget(std::make_shared<WidgetTextField>(137, 716, 391, 27, style, M("время разгона, сек")));
	addWidget(std::make_shared<WidgetTextField>(753, 716, 391, 27, style, M("время торможения, сек")));

	style.halign = Align::Left;
	style.text_rotate = -3.14 / 2;
	addWidget(std::make_shared<WidgetTextField>(30, 557, 247, 27, style, M("скорость, км/ч")));
	addWidget(std::make_shared<WidgetTextField>(646, 557, 247, 27, style, M("скорость, км/ч")));


	style.text_rotate = 0;
	style.halign = Align::Right;
	style.font_size = 50;
	style.text_color_disabled = to_base(Colors::White);
	style.update_func = [](auto w) {
			char buff[9];
			sprintf(buff, "% 4d", TreadmillDriver::getAccelTimeValue(ctx.m_accel));
			return std::string(buff);
		};
	addWidget(std::make_shared<WidgetTextField>(285, 168, 159, 45, style));

	style.update_func = [](auto w) {
			char buff[9];
			sprintf(buff, "%5.3f", ctx.m_treadmill.getAccelValue(ctx.m_accel) / 1000.0);
			return std::string(buff);
		};
	addWidget(std::make_shared<WidgetTextField>(285, 213, 159, 45, style));

	style.update_func = [](auto w) {
			char buff[9];
			sprintf(buff, "% 4d", TreadmillDriver::getAccelTimeValue(ctx.m_decel));
			return std::string(buff);
		};
	addWidget(std::make_shared<WidgetTextField>(936, 168, 159, 45, style));

	style.update_func = [](auto w) {
			char buff[9];
			sprintf(buff, "%5.3f", ctx.m_treadmill.getAccelValue(ctx.m_decel) / 1000.0);
			return std::string(buff);
		};
	addWidget(std::make_shared<WidgetTextField>(936, 213, 159, 45, style));

	switch (ctx.m_accel) {
	case 1:	m_accel_1->setTrigger(); break;
	case 2:	m_accel_2->setTrigger(); break;
	case 3:	m_accel_3->setTrigger(); break;
	case 4:	m_accel_4->setTrigger(); break;
	case 5:	m_accel_5->setTrigger(); break;
	case 6:	m_accel_6->setTrigger(); break;
	case 7:	m_accel_7->setTrigger(); break;
	}
	updateAccel();
	switch (ctx.m_decel) {
	case 1:	m_decel_1->setTrigger(); break;
	case 2:	m_decel_2->setTrigger(); break;
	case 3:	m_decel_3->setTrigger(); break;
	case 4:	m_decel_4->setTrigger(); break;
	case 5:	m_decel_5->setTrigger(); break;
	case 6:	m_decel_6->setTrigger(); break;
	case 7:	m_decel_7->setTrigger(); break;
	}
	updateDecel();

}

SettingsAccelMenu::~SettingsAccelMenu() {
	if (m_accel_prev != ctx.m_accel) {
		ctx.m_db.setParam("УровеньУскоренияДорожки", std::to_string(ctx.m_accel));
	}
	if (m_decel_prev != ctx.m_decel) {
		ctx.m_db.setParam("УровеньТорможенияДорожки", std::to_string(ctx.m_decel));
	}
	LD("Dtor");
}


void SettingsAccelMenu::updateAccel() {
	if (ctx.m_accel > 1) {
		m_accel_1->clearTrigger();
		m_accel_1->setPassive("settings_accel_selector_active.png");
		if (ctx.m_accel > 2) {
			m_accel_2->clearTrigger();
			m_accel_2->setPassive("settings_accel_selector_active.png");
			if (ctx.m_accel > 3) {
				m_accel_3->clearTrigger();
				m_accel_3->setPassive("settings_accel_selector_active.png");
				if (ctx.m_accel > 4) {
					m_accel_4->clearTrigger();
					m_accel_4->setPassive("settings_accel_selector_active.png");
					if (ctx.m_accel > 5) {
						m_accel_5->clearTrigger();
						m_accel_5->setPassive("settings_accel_selector_active.png");
						if (ctx.m_accel > 6) {
							m_accel_6->clearTrigger();
							m_accel_6->setPassive("settings_accel_selector_active.png");
						}
					}
				}
			}
		}
	}

	if (ctx.m_accel < 7) {
		m_accel_7->clearTrigger();
		m_accel_7->setPassive("");
		if (ctx.m_accel < 6) {
			m_accel_6->clearTrigger();
			m_accel_6->setPassive("");
			if (ctx.m_accel < 5) {
				m_accel_5->clearTrigger();
				m_accel_5->setPassive("");
				if (ctx.m_accel < 4) {
					m_accel_4->clearTrigger();
					m_accel_4->setPassive("");
					if (ctx.m_accel < 3) {
						m_accel_3->clearTrigger();
						m_accel_3->setPassive("");
						if (ctx.m_accel < 2) {
							m_accel_2->clearTrigger();
							m_accel_2->setPassive("");
						}
					}
				}
			}
		}
	}


	if (ctx.m_accel > 1) {
		m_accel_chart->setDisabled("settings_accel_" + std::to_string(ctx.m_accel) + "_active.png");
	} else {
		m_accel_chart->setDisabled("");
	}

	ctx.m_treadmill.setAcceleration(TreadmillDriver::getAccelTimeValue(ctx.m_accel) * 10);
}

void SettingsAccelMenu::updateDecel() {
	if (ctx.m_decel > 1) {
		m_decel_1->clearTrigger();
		m_decel_1->setPassive("settings_accel_selector_active.png");
		if (ctx.m_decel > 2) {
			m_decel_2->clearTrigger();
			m_decel_2->setPassive("settings_accel_selector_active.png");
			if (ctx.m_decel > 3) {
				m_decel_3->clearTrigger();
				m_decel_3->setPassive("settings_accel_selector_active.png");
				if (ctx.m_decel > 4) {
					m_decel_4->clearTrigger();
					m_decel_4->setPassive("settings_accel_selector_active.png");
					if (ctx.m_decel > 5) {
						m_decel_5->clearTrigger();
						m_decel_5->setPassive("settings_accel_selector_active.png");
						if (ctx.m_decel > 6) {
							m_decel_6->clearTrigger();
							m_decel_6->setPassive("settings_accel_selector_active.png");
						}
					}
				}
			}
		}
	}

	if (ctx.m_decel < 7) {
		m_decel_7->clearTrigger();
		m_decel_7->setPassive("");
		if (ctx.m_decel < 6) {
			m_decel_6->clearTrigger();
			m_decel_6->setPassive("");
			if (ctx.m_decel < 5) {
				m_decel_5->clearTrigger();
				m_decel_5->setPassive("");
				if (ctx.m_decel < 4) {
					m_decel_4->clearTrigger();
					m_decel_4->setPassive("");
					if (ctx.m_decel < 3) {
						m_decel_3->clearTrigger();
						m_decel_3->setPassive("");
						if (ctx.m_decel < 2) {
							m_decel_2->clearTrigger();
							m_decel_2->setPassive("");
						}
					}
				}
			}
		}
	}


	if (ctx.m_decel > 1) {
		m_decel_chart->setDisabled("settings_decel_" + std::to_string(ctx.m_decel) + "_active.png");
	} else {
		m_decel_chart->setDisabled("");
	}

	ctx.m_treadmill.setDeceleration(TreadmillDriver::getAccelTimeValue(ctx.m_decel) * 10);
}
