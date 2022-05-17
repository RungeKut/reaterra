#include "FreeRunMenuSas.h"

#include <cmath>

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "ModalSasParams.h"
#include "ModalSasResults.h"
#include "RehabMenu.h"
#include "SyncQueue.h"
#include "TouchEvent.h"
#include "Utils.h"

FreeRunMenuSas::FreeRunMenuSas()
:	Menu {0, 0, "sas_freerun_rehab_procedures_main_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "FR"));}, 0},
	m_lifting {false},
	m_refresh_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));}, 1},
	m_paused {false},
	m_cur_state {ctx.m_treadmill.getState()},
	m_hr_max {180},
	m_hr_crit {200},
	m_hr_alarm {0},
	m_sas_mode {SasMode::NoSas}
{
	LD("Ctor");
	std::string unit;

	ctx.m_treadmill.setSasMode(to_base(SasMode::NoSas));

	ctx.m_prev_menu["FreeRunMenuSas.HR"] = {
			std::to_string(m_hr_max),
			std::to_string(m_hr_crit),
			std::to_string(m_hr_alarm)};

	addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
			{M("свободный бег"), WIBFunc(), bc_last_style}
	});

	ctx.m_treadmill.setSpeed(10);
	ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
	ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 25;
	style.text_color_disabled = 0x434242;
	m_power_lbl = addWidget(std::make_shared<WidgetTextField>(169, 64, 301, 27, style, M("МОЩНОСТЬ")));
	style.halign = Align::Right;
	m_simmetry_lbl = addWidget(std::make_shared<WidgetTextField>(61, 64, 195, 27, style, M("СИММЕТРИЯ")));
	m_step_lbl = addWidget(std::make_shared<WidgetTextField>(382, 64, 105, 27, style, M("ШАГ")));
	addWidget(std::make_shared<WidgetTextField>(619, 64, 127, 27, style, M("ПУТЬ")));
	addWidget(std::make_shared<WidgetTextField>(832, 64, 144, 27, style, M("ЭНЕРГИЯ")));
	addWidget(std::make_shared<WidgetTextField>(1113, 64, 143, 27, style, M("ПУЛЬС")));
	style.text_color_disabled = 0x87CEEA;
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(564, 266, 699, 28, style, M("УПРАВЛЕНИЕ БЕГОВЫМ ПОЛОТНОМ")));
	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Right;
	style.font_size = 22;
	addWidget(std::make_shared<WidgetTextField>(0, 261, 112, 50, style, M("ВРЕМЯ\\БЕГА")));
	style.halign = Align::Left;
	style.font_size = 20;
	addWidget(std::make_shared<WidgetTextField>(1086, 390, 194, 23, style, M("АДАПТИВНЫЕ")));
	addWidget(std::make_shared<WidgetTextField>(1086, 428, 194, 23, style, M("РЕЖИМЫ")));
//	style.font_style = FontStyle::Normal;
	style.halign = Align::Center;
	style.font_size = 15;
	addWidget(std::make_shared<WidgetTextField>(138, 300, 79, 20, style, M("ЧАС")));
	addWidget(std::make_shared<WidgetTextField>(222, 300, 83, 20, style, M("МИН")));
	addWidget(std::make_shared<WidgetTextField>(308, 300, 79, 20, style, M("СЕК")));
//	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	addWidget(std::make_shared<WidgetTextField>(139, 348, 404, 25, style, M("НАПРАВЛЕНИЕ")));
//	addWidget(std::make_shared<WidgetTextField>(139, 480, 404, 25, style, M("ПУСК")));
	unit = ctx.m_db.getParam("ЕдиницыУгол");
	addWidget(std::make_shared<WidgetTextField>(139, 612, 404, 25, style, M("УГОЛ ПОДЪЕМА") + ", " + M(unit)));
	unit = ctx.m_db.getParam("ЕдиницыСкорость");
	addWidget(std::make_shared<WidgetTextField>(139, 480, 404, 25, style, M("СКОРОСТЬ") + ", " + M(unit)));

	style.text_color_active = to_base(Colors::White);
	style.text_color_passive = to_base(Colors::White);
	style.text_color_disabled = to_base(Colors::White);
	m_mode_free_run = addWidget(std::make_shared<WidgetImageButton>(623, 319,
				"sas_freerun_rehab_procedures_main_menu_mode_active.png",
				"sas_freerun_rehab_procedures_main_menu_mode_disable.png",
				"sas_freerun_rehab_procedures_main_menu_mode_passive.png",
				[this](WidgetImageButton::Event e) {
					sasModeUpdate(SasMode::NoSas);
					ctx.m_treadmill.setSpeed(10);
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(623, 333, 428, 24, style, M("РЕЖИМ СВОБОДНОГО БЕГА"))));
	m_mode_free_walk = addWidget(std::make_shared<WidgetImageButton>(623, 392,
				"sas_freerun_rehab_procedures_main_menu_mode_active.png",
				"sas_freerun_rehab_procedures_main_menu_mode_disable.png",
				"sas_freerun_rehab_procedures_main_menu_mode_passive.png",
				[this](WidgetImageButton::Event e) {
//					ctx.m_treadmill.setSpeed(1);
					sasModeUpdate(SasMode::SasFreeWalk);
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(623, 406, 428, 24, style, M("СВОБОДНАЯ ХОДЬБА"))));
	m_mode_6m_test = addWidget(std::make_shared<WidgetImageButton>(623, 457,
				"sas_freerun_rehab_procedures_main_menu_mode_active.png",
				"sas_freerun_rehab_procedures_main_menu_mode_disable.png",
				"sas_freerun_rehab_procedures_main_menu_mode_passive.png",
				[this](WidgetImageButton::Event e) {
//					ctx.m_treadmill.setSpeed(1);
					sasModeUpdate(SasMode::SasTest);
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(623, 473, 428, 24, style, M("6-ТИ МИНУТНЫЙ ТЕСТ"))));

	style.font_size = 20;
	m_mode_exo = addWidget(std::make_shared<WidgetImageButton>(623, 522,
				"sas_freerun_rehab_procedures_main_menu_mode_active.png",
				"sas_freerun_rehab_procedures_main_menu_mode_disable.png",
				"sas_freerun_rehab_procedures_main_menu_mode_passive.png",
				[this](WidgetImageButton::Event e) {
//					ctx.m_treadmill.setSpeed(1);
					sasModeUpdate(SasMode::SasExo);
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(623, 535, 428, 24, style, M("ПРЕРЫВИСТАЯ ХОДЬБА"))));

	m_hr_settings = addWidget(std::make_shared<WidgetImageButton>(1121, 483,
				"sas_freerun_rehab_procedures_main_menu_hrsettings_active.png",
				"sas_freerun_rehab_procedures_main_menu_hrsettings_disable.png",
				"sas_freerun_rehab_procedures_main_menu_hrsettings_passive.png",
				[this](WidgetImageButton::Event e) {
					m_modal = std::make_unique<ModalSasParams>();
					return nullptr;
				}, WidgetImageButton::Type::Clicker));


	style.text_color_disabled = 0x434242;
	style.font_size = 23;
	m_hr_max_widget = addWidget(std::make_shared<WidgetTextField>(1142, 163, 139, 27, style));
	m_hr_max_widget->setText(std::to_string(m_hr_max));
	m_hr_crit_widget = addWidget(std::make_shared<WidgetTextField>(1142, 213, 139, 27, style));
	m_hr_crit_widget->setText(std::to_string(m_hr_crit));

	//	addWidget(std::make_shared<WidgetTextField>(1096, 302, 184, 59, style, M("СТОП")));

	addWidget(std::make_shared<WidgetImageButton>(1064, 390, "",
			"sas_freerun_rehab_procedures_main_menu_bracket.png"))->setState(Widget::State::Disabled);
	m_power_pic = addWidget(std::make_shared<WidgetImageButton>(67, 58, "",
			"sas_freerun_rehab_procedures_main_menu_power.png"));
	m_power_pic ->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(245, 376, "",
			"sas_freerun_rehab_procedures_main_menu_greywindow.png"))->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(245, 509, "",
			"sas_freerun_rehab_procedures_main_menu_greywindow.png"))->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(245, 641, "",
			"sas_freerun_rehab_procedures_main_menu_greywindow.png"))->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(35, 371, "",
			"sas_freerun_rehab_procedures_main_menu_direction.png"))->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(42, 636, "",
			"sas_freerun_rehab_procedures_main_menu_angle.png"))->setState(Widget::State::Disabled);
	addWidget(std::make_shared<WidgetImageButton>(34, 513, "",
			"sas_freerun_rehab_procedures_main_menu_speed.png"))->setState(Widget::State::Disabled);
	m_div = addWidget(std::make_shared<WidgetImageButton>(279, 142, "",
			"sas_freerun_rehab_procedures_main_menu_div.png"));
	m_div->setState(Widget::State::Disabled);
	m_div_half = addWidget(std::make_shared<WidgetImageButton>(139, 142, "",
			"sas_freerun_rehab_procedures_main_menu_div.png"));
	m_div_half->setState(Widget::State::Disabled);
	m_div_whole = addWidget(std::make_shared<WidgetImageButton>(279, 46, "",
			"sas_freerun_rehab_procedures_main_menu_div_whole.png"));
	m_div_whole->setState(Widget::State::Disabled);
	m_simmetry_pic = addWidget(std::make_shared<WidgetImageButton>(13, 67, "",
			"sas_freerun_rehab_procedures_main_menu_simmetry.png"));
	m_simmetry_pic->setState(Widget::State::Disabled);
	m_step_pic = addWidget(std::make_shared<WidgetImageButton>(298, 71, "",
			"sas_freerun_rehab_procedures_main_menu_step.png"));
	m_step_pic->setState(Widget::State::Disabled);

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 31;
	style.text_color_disabled = 0x5A5A5A;
	style.update_func = [this](auto w) {
			if (ctx.m_treadmill.getDirection() == TreadmillDriver::Direction::Forward)
				return M("ВПЕРЁД");
			else
				return M("НАЗАД");
		};
	addWidget(std::make_shared<WidgetTextField>(245, 399, 192, 42, style));
	style.update_func = UpdateFunc();

	m_max_forward_speed = ctx.m_treadmill.getMaxForwardSpeed();
	m_max_backward_speed = ctx.m_treadmill.getMaxBackwardSpeed();
	m_speed_step = ctx.m_treadmill.getSpeedStep();
	m_min_speed = ctx.m_treadmill.getMinSpeed();

	style = default_style;
	style.kbd_type = KbdType::None;
	style.rect_color_disabled = to_base(Colors::None);
	style.lines = 1;
	style.font_size = 25;
	style.halign = Align::Right;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(1113, 98, 143, 26, style, M("удар/мин")));
	m_simmetry_dim = addWidget(std::make_shared<WidgetTextField>(61, 100, 195, 22, style, M("%")));
	m_step_dim = addWidget(std::make_shared<WidgetTextField>(413, 100, 73, 22, style, M("см")));

	style.font_size = 17;
	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	unit = M("МЕХАНИЧЕСКАЯ") + ", " + M(ctx.m_db.getParam("ЕдиницыМощность"));
	m_power_mech_lbl = addWidget(std::make_shared<WidgetTextField>(0, 214, 279, 20, style, unit));

	std::string un = ctx.m_db.getParam("ЕдиницыМощностьАэробная");
	unit = M("АЭРОБНАЯ") + ", " + (un == "MET" ? " " : "") + M(un);
	m_power_aero_lbl = addWidget(std::make_shared<WidgetTextField>(279, 214, 231, 20, style, unit));

	m_simmetry_left = addWidget(std::make_shared<WidgetTextField>(0, 214, 139, 20, style, M("ЛЕВАЯ")));
	m_simmetry_right = addWidget(std::make_shared<WidgetTextField>(139, 214, 140, 20, style, M("ПРАВАЯ")));
	addWidget(std::make_shared<WidgetTextField>(1142, 145, 139, 20, style, M("ЧСС МАКС")));
	addWidget(std::make_shared<WidgetTextField>(1142, 195, 139, 20, style, M("ЧСС КРИТ")));

	style.halign = Align::Right;
	style.font_size = 25;
	style.font_style = FontStyle::Normal;
	unit = ctx.m_db.getParam("ЕдиницыРасстояние");
	if (unit == "мили") {
		unit = M("миль");
	} else {
		unit = M(unit);
	}
	addWidget(std::make_shared<WidgetTextField>(619, 100, 127, 22, style, unit));

	unit = ctx.m_db.getParam("ЕдиницыЭнергия");
	addWidget(std::make_shared<WidgetTextField>(832, 100, 144, 22, style, M(unit)));

//	style.text_color_disabled = to_base(Colors::White);
//	unit = ctx.m_db.getParam("ЕдиницыУгол");
//	addWidget(std::make_shared<WidgetTextField>(477, 543, 135, 36, style, M(unit)));

//	unit = ctx.m_db.getParam("ЕдиницыСкорость");
//	addWidget(std::make_shared<WidgetTextField>(1053, 543, 135, 36, style, M(unit)));


	style.text_max_size = 6;
	style.font_size = 48;
	style.font_style = FontStyle::Bold;
	style.halign = Align::Left;

	style.text_color_disabled = DEC2RGB(204, 153, 51);
	style.update_func = [this](auto w) {
			return Utils::formatTime(ctx.m_session.getDuration());
		};
	addWidget(std::make_shared<WidgetTextField>(140, 251, 285, 49, style));


	style.font_size = 65;
	style.halign = Align::Center;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x3C3C3C;
	style.update_func = [this](auto w) {
			int hr = static_cast<int>(ctx.m_session.getHrate());
			return hr ? std::to_string(hr) : "- - -";
		};
	addWidget(std::make_shared<WidgetTextField>(1001, 156, 141, 55, style));

	style.update_func = [this](auto w) {
			return ctx.m_session.getDistanceString();
		};
	addWidget(std::make_shared<WidgetTextField>(511, 148, 260, 70, style));

	style.update_func = [this](auto w) {
			return ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run ? std::to_string(ctx.m_session.getSasSymmetryLeft()) : "0";
		};
	m_simmetry_left_value = addWidget(std::make_shared<WidgetTextField>(0, 156, 139, 55, style));

	style.update_func = [this](auto w) {
			return ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run ? std::to_string(ctx.m_session.getSasSymmetryRight()) : "0";
		};
	m_simmetry_right_value = addWidget(std::make_shared<WidgetTextField>(139, 156, 140, 55, style));

	style.update_func = [this](auto w) {
			return ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run ? std::to_string(ctx.m_session.getSasStepLength()) : "0";
		};
	m_step_value = addWidget(std::make_shared<WidgetTextField>(279, 156, 231, 55, style));

	style.update_func = [this](auto &w) {
			std::string str = ctx.m_session.getEnergyString();
			w.setFontSize(str.size() > 6 ? 50 : 65);
			return str;
		};
	addWidget(std::make_shared<WidgetTextField>(771, 156, 230, 55, style));

	if (ctx.m_patient.empty()) {
		style.text_color_disabled = DEC2RGB(190, 190, 200);
	}

	style.update_func = [this](auto w) {
			return ctx.m_session.getPowerMechString();
		};
	m_power_mech_value = addWidget(std::make_shared<WidgetTextField>(0, 156, 279, 55, style));

	style.update_func = [this](auto w) {
			return ctx.m_session.getPowerAeroString();
		};
	m_power_aero_value = addWidget(std::make_shared<WidgetTextField>(279, 156, 231, 55, style));

	spinner_style.font_style = FontStyle::Bold;
	spinner_style.font_size = 50;

	addWidget(std::make_shared<WidgetTextField>(245, 660, 192, 51, spinner_style))
		->setUpdateFunc(UpdateFunc {[this](auto w) {
				return ctx.m_session.getAngleString(-1, "%4.1f");
			}});

	unit = ctx.m_db.getParam("ЕдиницыСкорость");
	m_w_speed = std::make_shared<WidgetTextField>(245, 527, 192, 51, spinner_style);
	m_w_speed->setUpdateFunc([this](WidgetTextField &w) {
				if (ctx.m_treadmill.getDirection() == TreadmillDriver::Direction::Backward
						&& ctx.m_treadmill.getSpeed() > ctx.m_treadmill.getMaxBackwardSpeed()) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getMaxBackwardSpeed());
				}

				if (m_sas_mode == SasMode::SasExo) {
					return ctx.m_session.getSpeedString(ctx.m_treadmill.getSpeed() / 100., "%4.2f");
				} else {
					return ctx.m_session.getSpeedString(-1, "%4.2f");
				}

			}
	).setCheckFunc([this, unit](std::string s, WidgetTextField &w) {
				std:: string ret;
				if (s.empty()) {
					ret = "Значение не может быть пустым";
				} else {
					float speed = std::stof(s);
					if (unit == "м/с") {
						speed = Utils::ms2kmh(speed);
					} else if (unit == "миля/ч") {
						speed = Utils::milih2kmh(speed);
					}

					ctx.m_treadmill.setSpeed(speed * 10);

					this->updateSpeedButtons();
				}
				return ret;
			});

	m_w_speed->setState(Widget::State::Passive);
	addWidget(m_w_speed);

	m_start = std::make_shared<WidgetImageButton>(700, 600,
			"sas_freerun_rehab_procedures_start_active.png",
			"sas_freerun_rehab_procedures_start_disable.png",
			"sas_freerun_rehab_procedures_start_passive.png",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
					doPause();
				} else {
					doRun();
				}
				return nullptr;
		});
	addWidget(m_start);

	m_stop = std::make_shared<WidgetImageButton>(916, 600,
			"sas_freerun_rehab_procedures_stop_active.png",
			"sas_freerun_rehab_procedures_stop_disable.png",
			"sas_freerun_rehab_procedures_stop_passive.png",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getState() != TreadmillDriver::RunState::Stop) {
					doStop(false, true);
				}
				return nullptr;
		});
	addWidget(m_stop);

	m_plus_speed = std::make_shared<WidgetImageButton>(437, 508,
			"sas_freerun_rehab_procedures_plus_active.png",
			"sas_freerun_rehab_procedures_plus_disable.png",
			"sas_freerun_rehab_procedures_plus_passive.png",
			[this](WidgetImageButton::Event e) {

				if (ctx.m_treadmill.getSpeed() < m_max_speed) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getSpeed() + m_speed_step);
				}

				updateSpeedButtons();
				return nullptr;
		});
	m_plus_speed->setRepeat(1000, 200);
	addWidget(m_plus_speed);


	m_minus_speed = std::make_shared<WidgetImageButton>(139, 508,
			"sas_freerun_rehab_procedures_minus_active.png",
			"sas_freerun_rehab_procedures_minus_disable.png",
			"sas_freerun_rehab_procedures_minus_passive.png",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getSpeed() > m_min_speed) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getSpeed() - m_speed_step);
				}

				updateSpeedButtons();
				return nullptr;
		});
	m_minus_speed->setRepeat(1000, 200);
	addWidget(m_minus_speed);


	m_dir_backward = std::make_shared<WidgetImageButton>(139, 375,
			"sas_freerun_rehab_procedures_backward_active.png",
			"sas_freerun_rehab_procedures_backward_disable.png",
			"sas_freerun_rehab_procedures_backward_passive.png",
			[this](WidgetImageButton::Event e) {

				LD("Set direction backward");

				if (ctx.m_treadmill.getState() != TreadmillDriver::RunState::Run) {
					using Flags = ModalCommonDialog::Flags;
					m_modal = std::make_unique<ModalCommonDialog>(M("ВНИМАНИЕ! УСТАНОВЛЕНО\\ДВИЖЕНИЕ НАЗАД"),
							M("ОТСОЕДИНИТЕ ПЕРЕДНЮЮ\\ДУГУ БЕЗОПАСНОСТИ"), "CHDR", Flags::GreenTitle | Flags::CloseButton,
							"icon_treadmill_green.png");
					m_dir_forward->clearTrigger();
					ctx.m_treadmill.setDirection(TreadmillDriver::Direction::Backward);

					m_max_speed = m_max_backward_speed;
					updateSpeedButtons();
					if (ctx.m_treadmill.getSpeed() > m_max_speed) {
						ctx.m_treadmill.setSpeed(m_max_speed);
					}
					onDirectionChange(false);
				}
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	m_dir_backward->setSound(SoundInterface::Sound::Silent);
	addWidget(m_dir_backward);


	m_dir_forward = std::make_shared<WidgetImageButton>(437, 375,
			"sas_freerun_rehab_procedures_forward_active.png",
			"sas_freerun_rehab_procedures_forward_disable.png",
			"sas_freerun_rehab_procedures_forward_passive.png",
			[this](WidgetImageButton::Event e) {

			LD("Set direction forward");

			if (ctx.m_treadmill.getState() != TreadmillDriver::RunState::Run) {
					m_max_speed = m_max_forward_speed;
					ctx.m_treadmill.setDirection(TreadmillDriver::Direction::Forward);
					m_dir_backward->clearTrigger();
					updateSpeedButtons();
					onDirectionChange(true);
				}
				return nullptr;
		}, WidgetImageButton::Type::Trigger);
	addWidget(m_dir_forward);


	m_plus_angle = std::make_shared<WidgetImageButton>(437, 640,
			"sas_freerun_rehab_procedures_plus_active.png",
			"sas_freerun_rehab_procedures_plus_disable.png",
			"sas_freerun_rehab_procedures_plus_passive.png",
			[this](WidgetImageButton::Event e) {
				if (e == WidgetImageButton::Event::Press) {
					LD("plus ang pressed");
					if (!m_lifting && ctx.m_treadmill.getAngle() < ctx.m_settings.getParamInt("angle_max")) {
						m_lifting = true;
						ctx.m_treadmill.setLift(TreadmillDriver::Lift::Up);
					}
				} else {
					if (m_lifting) {
						m_lifting = false;
						ctx.m_treadmill.setLift(TreadmillDriver::Lift::Stop);
					}
					LD("plus ang released");
				}
				ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);
				return nullptr;
		}, WidgetImageButton::Type::PressRelease);
	addWidget(m_plus_angle);

	m_minus_angle = std::make_shared<WidgetImageButton>(139, 640,
			"sas_freerun_rehab_procedures_minus_active.png",
			"sas_freerun_rehab_procedures_minus_disable.png",
			"sas_freerun_rehab_procedures_minus_passive.png",
			[this](WidgetImageButton::Event e) {
				if (e == WidgetImageButton::Event::Press) {
					LD("minus ang pressed");
					if (!m_lifting && ctx.m_treadmill.getAngle() > ctx.m_settings.getParamInt("angle_min")) {
						m_lifting = true;
						ctx.m_treadmill.setLift(TreadmillDriver::Lift::Down);
					}
				} else {
					if (m_lifting) {
						m_lifting = false;
						ctx.m_treadmill.setLift(TreadmillDriver::Lift::Stop);
					}
					LD("minus ang released");
				}
				ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);
				return nullptr;
		}, WidgetImageButton::Type::PressRelease);
	addWidget(m_minus_angle);

	if (ctx.m_treadmill.getDirection() == TreadmillDriver::Direction::Forward) {
		m_dir_forward->setTrigger();
		onDirectionChange(true);
	} else {
		m_dir_backward->setTrigger();
		onDirectionChange(false);
	}

	if (ctx.m_treadmill.getSpeed() >= ctx.m_treadmill.getMaxForwardSpeed()) {
		m_plus_speed->setState(WidgetImageButton::State::Disabled);
	} else if (ctx.m_treadmill.getSpeed() <= m_min_speed) {
		m_minus_speed->setState(WidgetImageButton::State::Disabled);
	}

	if (ctx.m_treadmill.getAngle() <= ctx.m_settings.getParamInt("angle_min")) {
		m_minus_angle->setState(Widget::State::Disabled);
	}
	if (ctx.m_treadmill.getAngle() >= ctx.m_settings.getParamInt("angle_max")) {
		m_plus_angle->setState(Widget::State::Disabled);
	}

	m_mode_free_run->setPassive("sas_freerun_rehab_procedures_main_menu_mode_active.png");
	m_mode_free_run->setDisabled("sas_freerun_rehab_procedures_main_menu_mode_active.png");
	m_simmetry_lbl->setVisible(false);
	m_div_whole->setVisible(false);
	m_div_half->setVisible(false);
	m_step_lbl->setVisible(false);
	m_simmetry_pic->setVisible(false);
	m_step_pic->setVisible(false);
	m_simmetry_dim->setVisible(false);
	m_step_dim->setVisible(false);
	m_simmetry_left->setVisible(false);
	m_simmetry_right->setVisible(false);
	m_simmetry_left_value->setVisible(false);
	m_simmetry_right_value->setVisible(false);
	m_step_value->setVisible(false);
	m_stop->setPassive("sas_freerun_rehab_procedures_stop_active.png");
	ctx.m_treadmill.setRemoteControlLifting(true);
	m_refresh_timer.startTimerMillis(200);

	sasLimitationUpdate();
	LD("Ctor complete");
}

FreeRunMenuSas::~FreeRunMenuSas() {
	ctx.m_session.finish();
	ctx.m_session.init();
	m_refresh_timer.stopTimer();
	m_timer.stopTimer();
	ctx.m_treadmill.setSasMode(to_base(SasMode::NoSas));
	ctx.m_treadmill.setRemoteControlLifting(false);
	ctx.m_treadmill.setDirection(TreadmillDriver::Direction::Forward);
	ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
	ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
	ctx.m_treadmill.setSpeed(10);
	ctx.m_treadmill.doZeroPoint();
	LD("Dtor");
}

Menu* FreeRunMenuSas::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run && event->getType() == Event::EventType::Timer && event->getExtra() == "FR") {
		if (m_sas_mode == SasMode::NoSas || (ctx.m_treadmill.isFeetReady() && ctx.m_treadmill.isSasManOnTread())) {
			if (ctx.m_treadmill.getActualSpeed() > 0 || (m_sas_mode == SasMode::SasExo && ctx.m_session.getDuration() > 0)) {
				ctx.m_session.incSec();
			}
		}
		if (m_sas_mode == SasMode::SasTest && ctx.m_session.getDuration() >= 6 * 60) {
			doStop(false, true);
		}
	}

	ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);

	if (ctx.m_treadmill.getAngle() <= ctx.m_settings.getParamInt("angle_min") || m_sas_mode == SasMode::SasExo) {
		m_minus_angle->setState(Widget::State::Disabled);
	} else if (m_minus_angle->getState() == Widget::State::Disabled) {
		m_minus_angle->setState(Widget::State::Passive);
	}
	if (ctx.m_treadmill.getAngle() >= ctx.m_settings.getParamInt("angle_max") || m_sas_mode == SasMode::SasExo) {
		m_plus_angle->setState(Widget::State::Disabled);
	} else if (m_plus_angle->getState() == Widget::State::Disabled) {
		m_plus_angle->setState(Widget::State::Passive);
	}



	if (!m_remote_pult && !ctx.m_treadmill.isSettingPoint()) {
		m_remote_pult = true;
		LD("Set remote controle - true");
		ctx.m_treadmill.setRemoteControlLifting(true);
	}


	updateSpeedButtons();

	if (m_modal && m_modal->getType() == "CHDR" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}


	if (m_modal && m_modal->getType() == "FTST" && event->getType() == Event::EventType::ModalCancel) {
		doStop(false);
	}


	if (m_modal && m_modal->getType() == "HRP" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		m_hr_max = stoi(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(0));
		m_hr_max_widget->setText(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(0));
		m_hr_crit = stoi(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(1));
		m_hr_crit_widget->setText(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(1));
		m_hr_alarm = stoi(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(2));
	}

	TreadmillDriver::RunState state = ctx.m_treadmill.getState();
	if (m_cur_state != state) {
		using  RState = TreadmillDriver::RunState;

		if (m_cur_state == RState::Stop && (state == RState::Run || state == RState::Pause)) {
			doRun(true);
		} else if (m_cur_state == RState::Pause && state == RState::Run) {
			doRun(true);
		}
		if (state == RState::Pause) {
			doPause(true);
		} else if (state == RState::Stop) {
			doStop(true);
		}

		m_cur_state = state;
	}


	if (ctx.m_session.getHrate() >= m_hr_crit) {
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::make_unique<ModalCommonDialog>(M("ПРЕВЫШЕНО КРИТИЧЕСКОЕ\\ЗНАЧЕНИЕ ЧСС"),
				M("БЕГ ОСТАНОВЛЕН"), "HRE2", Flags::YellowTitle | Flags::CloseButton,
				"icon_hr_exclamation_yellow.png");
		doStop(true);
	}


	if (m_sas_mode != SasMode::NoSas) {

		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run && ctx.m_treadmill.isFeetReady()
				&& ctx.m_treadmill.isSasManOnTread() && ctx.m_session.getSasStepLength() == 0)
		{
			if (!m_modal && m_start_left_show) {
				m_start_left_show = false;
				using Flags = ModalCommonDialog::Flags;
				std::unique_ptr<ModalCommonDialog> no_hr_modal = std::make_unique<ModalCommonDialog>(M("ВНИМАНИЕ!"),
						M("НАЧИНАЙТЕ ХОДЬБУ\\С ЛЕВОЙ НОГИ"), "WRLG", Flags::GreenTitle | Flags::CloseButton,
						"icon_treadmill_green.png");

				m_modal = std::move(no_hr_modal);
			}
		} else if (m_modal && m_modal->getType() == "WRLG") {
			m_modal.reset();
		}


		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run && (!ctx.m_treadmill.isFeetReady() || !ctx.m_treadmill.isSasManOnTread())) {
			if (!m_modal) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("ПОСТАВЬТЕ НОГИ РЯДОМ,\\КАК ПОКАЗАНО НА РИСУНКЕ"),
						M("ДЛЯ ОТМЕНЫ НАЖМИТЕ КНОПКУ"), "FTST", Flags::GreenTitle | Flags::Cancel,
						"icon_feet_green.png");
			}
		} else if (m_modal && m_modal->getType() == "FTST") {
			m_modal.reset();
		}

	}


	ret = Menu::processEvent(std::move(event));
	if (m_event != EventType::None) {
//		LD("TState event: " + (state == TreadmillDriver::RunState::Run ? "run" : "not run"));
		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
			if (m_event == EventType::Stop) {
				doStop(false);
			} else if (m_event == EventType::Pause) {
				doPause();
			}
		}
	}
	return ret;
}

void FreeRunMenuSas::updateSpeedButtons() {
	if (m_sas_mode != SasMode::NoSas) {
		return;
	}
	if (ctx.m_treadmill.getSpeed() >= m_max_speed) {
		if (m_plus_speed->getState() != Widget::State::Disabled) {
			m_plus_speed->setState(WidgetImageButton::State::Disabled);
		}
	} else {
		if (m_plus_speed->getState() == Widget::State::Disabled) {
			m_plus_speed->setState(Widget::State::Passive);
		}
	}

	if (ctx.m_treadmill.getSpeed() <= m_min_speed) {
		if (m_minus_speed->getState() != Widget::State::Disabled) {
			m_minus_speed->setState(WidgetImageButton::State::Disabled);
		}
	} else {
		if (m_minus_speed->getState() == Widget::State::Disabled) {
			m_minus_speed->setState(WidgetImageButton::State::Passive);
		}
	}
}

void FreeRunMenuSas::onDirectionChange(bool forward) {
	if (forward) {
		m_max_speed = m_max_forward_speed;
	} else {
		m_max_speed = m_max_backward_speed;
	}

	float min_speed_in_units = m_min_speed / 10.;
	float max_speed_in_units = m_max_speed / 10.;

	std::string unit = ctx.m_db.getParam("ЕдиницыСкорость");
	if (unit == "м/с") {
		min_speed_in_units = Utils::kmh2ms(min_speed_in_units);
		max_speed_in_units = Utils::kmh2ms(max_speed_in_units);
	} else if (unit == "миля/ч") {
		min_speed_in_units = Utils::kmh2milih(min_speed_in_units);
		max_speed_in_units = Utils::kmh2milih(max_speed_in_units);
	}

	if (m_w_speed) {
		m_w_speed->setLimits(std::round(min_speed_in_units * 100) / 100, std::round(max_speed_in_units * 100) / 100, M("Ввод числа\\от %4.2f\\до %4.2f"));
	}
}

void FreeRunMenuSas::doRun(bool remote) {
	if (m_paused) {
		m_paused = false;
	} else {
		std::string sas_mode = "";
		if (m_sas_mode == SasMode::SasExo) {
			sas_mode = "Прер. ходьба";
		} else if (m_sas_mode == SasMode::SasFreeWalk) {
			sas_mode = "Своб. ходьба";
		} else if (m_sas_mode == SasMode::SasTest) {
			sas_mode = "6 мин. тест";
		}
		ctx.m_session.start(ctx.m_patient_id, M("Свободный бег"), sas_mode);
	}
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Run, false);
	}
	m_cur_state = TreadmillDriver::RunState::Run;
	m_start->setPassive("sas_freerun_rehab_procedures_start_active.png");
	m_start->setActive("sas_freerun_rehab_procedures_start_active.png");
	m_stop->setPassive("sas_freerun_rehab_procedures_stop_passive.png");
	m_dir_backward->setState(Widget::State::Disabled);
	m_dir_forward->setState(Widget::State::Disabled);
	m_mode_6m_test->setState(Widget::State::Disabled);
	m_mode_exo->setState(Widget::State::Disabled);
	m_mode_free_run->setState(Widget::State::Disabled);
	m_mode_free_walk->setState(Widget::State::Disabled);
	m_hr_settings->setState(Widget::State::Disabled);
	m_timer.startTimerMillis(1000);
}

void FreeRunMenuSas::doPause(bool remote) {
	m_start_left_show = true;
	m_paused = true;
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause, false);
	}
	m_start->setPassive("sas_freerun_rehab_procedures_pause_active.png");
	m_start->setActive("sas_freerun_rehab_procedures_pause_active.png");
//	m_dir_backward->setState(Widget::State::Passive);
//	m_dir_forward->setState(Widget::State::Passive);
//	m_mode_6m_test->setState(Widget::State::Passive);
//	m_mode_exo->setState(Widget::State::Passive);
//	m_mode_free_run->setState(Widget::State::Passive);
//	m_mode_free_walk->setState(Widget::State::Passive);
	m_hr_settings->setState(Widget::State::Passive);
	m_stop->setPassive("sas_freerun_rehab_procedures_stop_passive.png");
	m_timer.stopTimer();
	m_cur_state = TreadmillDriver::RunState::Pause;
}

void FreeRunMenuSas::doStop(bool remote, bool info) {
	m_start_left_show = true;
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop, false);
	}
	ctx.m_session.finish();
	if (info && m_sas_mode != SasMode::NoSas && !m_modal) {
		std::string unit = ctx.m_db.getParam("ЕдиницыРасстояние");
		if (unit == "мили") {
			unit = M("миль");
		} else {
			unit = M(unit);
		}

		ctx.m_prev_menu["FreeRunMenuSas.RESULTS"] = {
				Utils::formatTime(ctx.m_session.getDuration()),
				std::to_string(ctx.m_session.getSasSymmetryLeftAvg()),  // Симметричность левая
				std::to_string(ctx.m_session.getSasSymmetryRightAvg()), // Симметричность правая
				std::to_string(ctx.m_session.getSasStepLengthAvg()) + " " + M("см"),    // Средняя длина шага
				ctx.m_session.getDistanceString() + " " + unit // Пройденное растояние

		};
		m_modal = std::make_unique<ModalSasResults>();
	}

	ctx.m_session.init();
	m_paused = false;
	m_timer.stopTimer();
	m_start->setPassive("sas_freerun_rehab_procedures_start_passive.png");
	m_start->setActive("sas_freerun_rehab_procedures_start_active.png");
	m_stop->setPassive("sas_freerun_rehab_procedures_stop_active.png");
	m_dir_backward->setState(Widget::State::Passive);
	m_dir_forward->setState(Widget::State::Passive);
	m_mode_6m_test->setState(Widget::State::Passive);
	m_mode_exo->setState(Widget::State::Passive);
	m_mode_free_run->setState(Widget::State::Passive);
	m_mode_free_walk->setState(Widget::State::Passive);
	m_hr_settings->setState(Widget::State::Passive);
	m_cur_state = TreadmillDriver::RunState::Stop;

	sasLimitationUpdate();
}

void FreeRunMenuSas::sasModeUpdate(SasMode smode) {
	bool noSas = (smode == SasMode::NoSas);
	bool noSasExo = (smode == SasMode::NoSas) || (smode == SasMode::SasExo);
	m_sas_mode = smode;
	ctx.m_treadmill.setSasMode(to_base(m_sas_mode));

	std::string pic_active = "sas_freerun_rehab_procedures_main_menu_mode_active.png";
	std::string pic_passive = "sas_freerun_rehab_procedures_main_menu_mode_passive.png";
	std::string pic_disabled = "sas_freerun_rehab_procedures_main_menu_mode_disable.png";

	m_mode_free_run->setPassive(smode == SasMode::NoSas ? pic_active : pic_passive);
	m_mode_free_walk->setPassive(smode == SasMode::SasFreeWalk ? pic_active : pic_passive);
	m_mode_6m_test->setPassive(smode == SasMode::SasTest ? pic_active : pic_passive);
	m_mode_exo->setPassive(smode == SasMode::SasExo ? pic_active : pic_passive);

	m_mode_free_run->setDisabled(smode == SasMode::NoSas ? pic_active : pic_disabled);
	m_mode_free_walk->setDisabled(smode == SasMode::SasFreeWalk ? pic_active : pic_disabled);
	m_mode_6m_test->setDisabled(smode == SasMode::SasTest ? pic_active : pic_disabled);
	m_mode_exo->setDisabled(smode == SasMode::SasExo ? pic_active : pic_disabled);

	m_power_lbl->setVisible(noSasExo);
	m_div->setVisible(noSasExo);
	m_power_pic->setVisible(noSasExo);
	m_power_mech_lbl->setVisible(noSasExo);
	m_power_aero_lbl->setVisible(noSasExo);
	m_power_mech_value->setVisible(noSasExo);
	m_power_aero_value->setVisible(noSasExo);

	m_simmetry_lbl->setVisible(!noSasExo);
	m_div_whole->setVisible(!noSasExo);
	m_div_half->setVisible(!noSasExo);
	m_step_lbl->setVisible(!noSasExo);
	m_simmetry_pic->setVisible(!noSasExo);
	m_step_pic->setVisible(!noSasExo);
	m_simmetry_dim->setVisible(!noSasExo);
	m_step_dim->setVisible(!noSasExo);
	m_simmetry_left->setVisible(!noSasExo);
	m_simmetry_right->setVisible(!noSasExo);
	m_simmetry_left_value->setVisible(!noSasExo);
	m_simmetry_right_value->setVisible(!noSasExo);
	m_step_value->setVisible(!noSasExo);

	if (smode == SasMode::SasExo) {
		ctx.m_treadmill.setRemoteControlLifting(false);
		m_remote_pult = false;
		if (ctx.m_treadmill.getState() != TreadmillDriver::RunState::Run && ctx.m_treadmill.getDirection() != TreadmillDriver::Direction::Forward) {
				m_max_speed = m_max_forward_speed;
				ctx.m_treadmill.setDirection(TreadmillDriver::Direction::Forward);
				m_dir_backward->clearTrigger();
				m_dir_forward->setTrigger();
				onDirectionChange(true);
		}
		ctx.m_treadmill.doZeroPoint();
	} else {
		ctx.m_treadmill.setRemoteControlLifting(true);
	}

	Widget::State wstate = (noSas ? Widget::State::Passive : Widget::State::Disabled);
	m_plus_speed->setState(wstate);
	m_minus_speed->setState(wstate);
	m_w_speed->setState(wstate);

	wstate = (smode != SasMode::SasExo ? Widget::State::Passive : Widget::State::Disabled);
	m_dir_backward->setState(wstate);
	m_dir_forward->setState(wstate);
	m_plus_angle->setState(wstate);
	m_minus_angle->setState(wstate);

	if (noSas) {
		ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
		ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
	} else {
		ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(7) * 10);
		ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(7) * 10);
	}

	sasLimitationUpdate();

}

void FreeRunMenuSas::sasLimitationUpdate() {
	if (ctx.m_treadmill.isSasTestCompletedWithLimitation()) {
		m_mode_6m_test->setState(Widget::State::Disabled);
		m_mode_exo->setState(Widget::State::Disabled);
		m_mode_free_walk->setState(Widget::State::Disabled);
	}
}
