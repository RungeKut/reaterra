#include "FreeRunMenu.h"

#include <cmath>

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "RehabMenu.h"
#include "SyncQueue.h"
#include "TouchEvent.h"
#include "Utils.h"

FreeRunMenu::FreeRunMenu()
:	Menu {0, 0, "freerun_rehab_procedures_main_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "FR"));}, 0},
	m_lifting {false},
	m_refresh_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));}, 1},
	m_paused {false},
	m_cur_state {ctx.m_treadmill.getState()}
{
	LD("Ctor");

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
	style.halign = Align::Left;
	style.font_style = FontStyle::Bold;
	style.font_size = 24;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(182, 48, 307, 59, style, M("МОЩНОСТЬ")));
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(512, 48, 256, 59, style, M("РАССТОЯНИЕ")));
	addWidget(std::make_shared<WidgetTextField>(835, 48, 190, 59, style, M("ЭНЕРГИЯ")));
	addWidget(std::make_shared<WidgetTextField>(1105, 48, 175, 59, style, M("ПУЛЬС")));
	style.text_color_disabled = 0x87CEEA;
	addWidget(std::make_shared<WidgetTextField>(419, 246, 861, 59, style, M("УПРАВЛЕНИЕ БЕГОВЫМ ПОЛОТНОМ")));
	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(108, 301, 293, 59, style, M("ВРЕМЯ БЕГА")));
	style.font_style = FontStyle::Normal;
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(24, 410, 293, 59, style, M("ч:мин:с")));
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(450, 302, 364, 59, style, M("НАПРАВЛЕНИЕ")));
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(880, 302, 196, 59, style, M("ПУСК")));
	addWidget(std::make_shared<WidgetTextField>(152, 531, 322, 59, style, M("УГОЛ ПОДЪЕМА")));
	addWidget(std::make_shared<WidgetTextField>(728, 531, 322, 59, style, M("СКОРОСТЬ")));
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(1096, 302, 184, 59, style, M("СТОП")));

	m_max_forward_speed = ctx.m_treadmill.getMaxForwardSpeed();
	m_max_backward_speed = ctx.m_treadmill.getMaxBackwardSpeed();
	m_speed_step = ctx.m_treadmill.getSpeedStep();
	m_min_speed = ctx.m_treadmill.getMinSpeed();

	style = default_style;
	style.kbd_type = KbdType::None;
	style.rect_color_disabled = to_base(Colors::None);
	style.lines = 1;
	style.font_size = 25;
	style.halign = Align::Center;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(1110, 94, 170, 36, style, M("удар/мин")));

	std::string unit {M("механическая") + ", " + M(ctx.m_db.getParam("ЕдиницыМощность"))};
	addWidget(std::make_shared<WidgetTextField>(0, 107, 252, 42, style, unit));

	std::string un = ctx.m_db.getParam("ЕдиницыМощностьАэробная");
	unit = M("аэробная") + "," + (un == "MET" ? " " : "") + M(un);
	addWidget(std::make_shared<WidgetTextField>(242, 107, 281, 42, style, unit));

	unit = ctx.m_db.getParam("ЕдиницыРасстояние");
	if (unit == "мили") {
		unit = M("миль");
	} else {
		unit = M(unit);
	}
	addWidget(std::make_shared<WidgetTextField>(610, 94, 159, 36, style, unit));

	unit = ctx.m_db.getParam("ЕдиницыЭнергия");
	addWidget(std::make_shared<WidgetTextField>(839, 94, 185, 36, style, M(unit)));

	style.text_color_disabled = to_base(Colors::White);
	unit = ctx.m_db.getParam("ЕдиницыУгол");
	addWidget(std::make_shared<WidgetTextField>(477, 543, 135, 36, style, M(unit)));

	unit = ctx.m_db.getParam("ЕдиницыСкорость");
	addWidget(std::make_shared<WidgetTextField>(1053, 543, 135, 36, style, M(unit)));


	style.text_max_size = 6;
	style.font_size = 70;

	style.text_color_disabled = DEC2RGB(204, 153, 51);
	style.update_func = [this](auto w) {
			return Utils::formatTime(ctx.m_session.getDuration());
		};
	addWidget(std::make_shared<WidgetTextField>(0, 354, 343, 69, style));


	style.font_size = 65;
	style.text_color_disabled = 0x3C3C3C;
	style.update_func = [this](auto w) {
			int hr = static_cast<int>(ctx.m_session.getHrate());
			return hr ? std::to_string(hr) : "- - -";
		};
	addWidget(std::make_shared<WidgetTextField>(1025, 148, 255, 94, style));

	style.update_func = [this](auto w) {
			return ctx.m_session.getDistanceString();
		};
	addWidget(std::make_shared<WidgetTextField>(492, 148, 296, 94, style));

	if (ctx.m_patient.empty()) {
		style.text_color_disabled = DEC2RGB(190, 190, 200);
	}

	style.update_func = [this](auto w) {
			return ctx.m_session.getPowerMechString();
		};
	addWidget(std::make_shared<WidgetTextField>(0, 148, 252, 94, style));

	style.update_func = [this](auto w) {
			return ctx.m_session.getPowerAeroString();
		};
	addWidget(std::make_shared<WidgetTextField>(232, 148, 301, 94, style));

	style.update_func = [this](auto &w) {
			std::string str = ctx.m_session.getEnergyString();
			w.setFontSize(str.size() > 6 ? 50 : 65);
			return str;
		};
	addWidget(std::make_shared<WidgetTextField>(749, 148, 296, 94, style));

	addWidget(std::make_shared<WidgetTextField>(284, 586, 209, 98, spinner_style))
		->setUpdateFunc(UpdateFunc {[this](auto w) {
				return ctx.m_session.getAngleString(-1, "%4.1f");
			}});

	m_w_speed = std::make_shared<WidgetTextField>(913, 587, 208, 97, spinner_style);
	m_w_speed->setUpdateFunc([unit](WidgetTextField &w) {
				if (ctx.m_treadmill.getDirection() == TreadmillDriver::Direction::Backward
						&& ctx.m_treadmill.getSpeed() > ctx.m_treadmill.getMaxBackwardSpeed()) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getMaxBackwardSpeed());
				}
				return ctx.m_session.getSpeedString(-1, "%4.2f");
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

	m_start = std::make_shared<WidgetImageButton>(928, 356, "freerun_rehab_procedures_start.png", "", "",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
					doPause();
				} else {
					doRun();
				}
				return nullptr;
		});
	addWidget(m_start);

	m_stop = std::make_shared<WidgetImageButton>(1086, 356, "freerun_rehab_procedures_stop.png", "", "freerun_rehab_procedures_stop.png",
			[this](WidgetImageButton::Event e) {
				doStop();
				return nullptr;
		});
	addWidget(m_stop);

	m_plus_speed = std::make_shared<WidgetImageButton>(1121, 585, "freerun_rehab_procedures_plus.png", "freerun_rehab_procedures_plus_disabled.png", "",
			[this](WidgetImageButton::Event e) {

				if (ctx.m_treadmill.getSpeed() < m_max_speed) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getSpeed() + m_speed_step);
				}

				updateSpeedButtons();
				return nullptr;
		});
	m_plus_speed->setRepeat(1000, 200);
	addWidget(m_plus_speed);


	m_minus_speed = std::make_shared<WidgetImageButton>(793, 585, "freerun_rehab_procedures_minus.png", "freerun_rehab_procedures_minus_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getSpeed() > m_min_speed) {
					ctx.m_treadmill.setSpeed(ctx.m_treadmill.getSpeed() - m_speed_step);
				}

				updateSpeedButtons();
				return nullptr;
		});
	m_minus_speed->setRepeat(1000, 200);
	addWidget(m_minus_speed);


	m_dir_backward = std::make_shared<WidgetImageButton>(404, 356, "freerun_rehab_procedures_backward.png", "freerun_rehab_procedures_backward_disable.png", "",
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


	m_dir_forward = std::make_shared<WidgetImageButton>(702, 356, "freerun_rehab_procedures_forward.png", "freerun_rehab_procedures_forward_disable.png", "",
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


	m_plus_angle = std::make_shared<WidgetImageButton>(493, 585, "freerun_rehab_procedures_plus.png", "freerun_rehab_procedures_plus_disabled.png", "",
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
//				ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);
				return nullptr;
		}, WidgetImageButton::Type::PressRelease);
	addWidget(m_plus_angle);

	m_minus_angle = std::make_shared<WidgetImageButton>(165, 585, "freerun_rehab_procedures_minus.png", "freerun_rehab_procedures_minus_disabled.png", "",
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
//				ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);
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

	ctx.m_treadmill.setRemoteControlLifting(true);
	m_refresh_timer.startTimerMillis(200);
	LD("Ctor complete");
}

FreeRunMenu::~FreeRunMenu() {
	ctx.m_session.finish();
	m_refresh_timer.stopTimer();
	m_timer.stopTimer();
	ctx.m_treadmill.setRemoteControlLifting(false);
	ctx.m_treadmill.setDirection(TreadmillDriver::Direction::Forward);
	ctx.m_treadmill.setSpeed(10);
	ctx.m_treadmill.doZeroPoint();
	LD("Dtor");
}

Menu* FreeRunMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "FR") {
		ctx.m_session.incSec();
	}

//	ctx.m_treadmill.setAngle(ctx.m_treadmill.getAngle(), true);

	if (ctx.m_treadmill.getAngle() <= ctx.m_settings.getParamInt("angle_min")) {
		m_minus_angle->setState(Widget::State::Disabled);
	} else if (m_minus_angle->getState() == Widget::State::Disabled) {
		m_minus_angle->setState(Widget::State::Passive);
	}
	if (ctx.m_treadmill.getAngle() >= ctx.m_settings.getParamInt("angle_max")) {
		m_plus_angle->setState(Widget::State::Disabled);
	} else if (m_plus_angle->getState() == Widget::State::Disabled) {
		m_plus_angle->setState(Widget::State::Passive);
	}

	updateSpeedButtons();

	if (m_modal && m_modal->getType() == "CHDR" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
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

	ret = Menu::processEvent(std::move(event));
	if (m_event != EventType::None) {
//		LD("TState event: " + (state == TreadmillDriver::RunState::Run ? "run" : "not run"));
		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
			if (m_event == EventType::Stop) {
				doStop();
			} else if (m_event == EventType::Pause) {
				doPause();
			}
		}
	}
	return ret;
}

void FreeRunMenu::updateSpeedButtons() {
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

void FreeRunMenu::onDirectionChange(bool forward) {
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

void FreeRunMenu::doRun(bool remote) {
	if (m_paused) {
		m_paused = false;
	} else {
		ctx.m_session.start(ctx.m_patient_id, M("Свободный бег"));
	}
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Run, false);
	}
	m_cur_state = TreadmillDriver::RunState::Run;
	m_start->setPassive("freerun_rehab_procedures_start.png");
	m_start->setActive("freerun_rehab_procedures_start.png");
	m_dir_backward->setState(Widget::State::Disabled);
	m_dir_forward->setState(Widget::State::Disabled);
	m_stop->setPassive("");
	m_timer.startTimerMillis(1000);
}

void FreeRunMenu::doPause(bool remote) {
	m_paused = true;
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause, false);
	}
	m_start->setPassive("freerun_rehab_procedures_pause.png");
	m_start->setActive("freerun_rehab_procedures_pause.png");
	m_dir_backward->setState(Widget::State::Passive);
	m_dir_forward->setState(Widget::State::Passive);
	m_stop->setPassive("");
	m_timer.stopTimer();
	m_cur_state = TreadmillDriver::RunState::Pause;
}

void FreeRunMenu::doStop(bool remote) {
	ctx.m_session.finish();
	if (!remote) {
		ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop, false);
	}
	m_timer.stopTimer();
	m_start->setPassive("");
	m_start->setActive("freerun_rehab_procedures_start.png");
	m_stop->setPassive("freerun_rehab_procedures_stop.png");
	m_dir_backward->setState(Widget::State::Passive);
	m_dir_forward->setState(Widget::State::Passive);
	m_cur_state = TreadmillDriver::RunState::Stop;
}
