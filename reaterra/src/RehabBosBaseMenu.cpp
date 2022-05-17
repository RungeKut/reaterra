#include "RehabBosBaseMenu.h"

#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "ModalWarmUpParams.h"
#include "RehabMenu.h"
#include "RehabSelHrMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"


RehabBosBaseMenu::RehabBosBaseMenu(Mode mode)
:	Menu {0, 0, "rehab_procedures_hr_minmax_menu.png"},
	m_total_time {0},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_warming_up {false},
	m_paused {false},
	m_interval_cnt {0},
	m_cur_state {ctx.m_treadmill.getState()}
{
	LD("Ctor");

	m_mode = mode;
	std::string title;
	std::string name_in_history;
	if (mode == Mode::MinMax) {
		title = M("бег с БОС задание ЧСС мин/макс");
		name_in_history = M("БОС ЧСС мин/макс");
	} else {
		title = M("бег с БОС задание по мощности");
		name_in_history = M("БОС по мощности");
	}

	m_warm_start_speed = 2.5;
	m_warm_end_speed = 2.5;
	m_max_speed = 5;
	m_n_speed = 1;

	m_first_interval= true;

	m_warm_time = 0;

	ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"] = {
			ctx.m_session.getSpeedString(m_warm_start_speed),
			ctx.m_session.getSpeedString(m_warm_end_speed),
			"1"};

	ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
	ctx.m_treadmill.setAngle(0);

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{title, WIBFunc(), bc_last_style}
	});

	m_extra_line = addWidget(std::make_shared<WidgetImageButton>(0, 553, "", "rehab_procedures_hr_extra_line.png", ""));
	m_extra_line->setState(Widget::State::Disabled)->setVisible(false);
	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = 0x0966CB;
	style.font_size = 25;
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(0, 47, 678, 52, style, M("ПАРАМЕТРЫ ПРОЦЕДУРЫ С БОС")));

	style.text_color_disabled = 0x87CEEA;
	addWidget(std::make_shared<WidgetTextField>(678, 47, 602, 52, style, M("УПРАВЛЕНИЕ ПРОЦЕДУРОЙ С БОС")));

	style.halign = Align::Left;
	style.text_color_disabled = 0x434242;
	style.font_size = 22;
	style.font_style = FontStyle::Normal;
	m_label_style = style;
	addWidget(std::make_shared<WidgetTextField>(5, 99, 407, 55, style, M("Проведение разминки")));
	addWidget(std::make_shared<WidgetTextField>(5, 155, 407, 56, style, M("Максимальная скорость")));
	WidgetTextFieldSP speed_txt = addWidget(std::make_shared<WidgetTextField>(5, 212, 407, 56, style, M("Начальная скорость")));
	addWidget(std::make_shared<WidgetTextField>(5, 269, 407, 56, style, M("Максимальный угол")));
	WidgetTextFieldSP angle_txt = addWidget(std::make_shared<WidgetTextField>(5, 326, 407, 56, style, M("Номинальный угол")));
	addWidget(std::make_shared<WidgetTextField>(5, 383, 407, 56, m_label_style, M("Максимально допустимая ЧСС")));

	m_proc_time_label = addWidget(std::make_shared<WidgetTextField>(5, 497, 407, 56, m_label_style, M("Время процедуры")));

	std::string speed_unit_str = ctx.m_db.getParam("ЕдиницыСкорость");
	style.halign = Align::Center;
	m_unit_style = style;
	addWidget(std::make_shared<WidgetTextField>(524, 155, 154, 56, style, M(speed_unit_str))); // Единица измерения параметра "Начальная скорость"
	addWidget(std::make_shared<WidgetTextField>(524, 212, 154, 56, style, M(speed_unit_str))); // Единица измерения параметра "Конечная скорость"
	addWidget(std::make_shared<WidgetTextField>(524, 269, 154, 56, style, M(ctx.m_db.getParam("ЕдиницыУгол")))); // Единица измерения параметра "Максимальный угол"
	addWidget(std::make_shared<WidgetTextField>(524, 326, 154, 56, style, M(ctx.m_db.getParam("ЕдиницыУгол")))); // Единица измерения параметра "Номинальный угол"
	addWidget(std::make_shared<WidgetTextField>(524, 383, 154, 56, m_unit_style, M("уд/мин"))); // Единица измерения параметра "Максимально допустимая ЧСС"

	m_proc_time_unit_label = addWidget(std::make_shared<WidgetTextField>(524, 497, 154, 56, m_unit_style, M("мин"))); // Единица измерения параметра "Время процедуры"

	style.halign = Align::Center;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(1156, 98, 124, 56, style, M("уд/мин"))); // Единица измерения параметра "Измеренное значение ЧСС"
	addWidget(std::make_shared<WidgetTextField>(1156, 155, 124, 56, style, M(ctx.m_db.getParam("ЕдиницыЭнергия")))); // Единица измерения параметра "Затраченная энергия"
	addWidget(std::make_shared<WidgetTextField>(1156, 212, 124, 56, style, M(ctx.m_db.getParam("ЕдиницыМощность")))); // Единица измерения параметра "Текущая мощность"
	addWidget(std::make_shared<WidgetTextField>(1156, 269, 124, 56, style, M(ctx.m_db.getParam("ЕдиницыМощностьАэробная")))); // Единица измерения параметра "Текущая аэробная мощность"
	addWidget(std::make_shared<WidgetTextField>(1156, 326, 124, 56, style, M("ч:мин:с"))); // Единица измерения параметра "Текущее время процедуры"
	addWidget(std::make_shared<WidgetTextField>(1156, 383, 124, 56, style, M("ч:мин:с"))); // Единица измерения параметра "Общее время процедуры"
	addWidget(std::make_shared<WidgetTextField>(1156, 440, 124, 56, style, M(speed_unit_str))); // Единица измерения параметра "Текущая скорость"
	addWidget(std::make_shared<WidgetTextField>(1156, 497, 124, 56, style, M(ctx.m_db.getParam("ЕдиницыУгол")))); // Единица измерения параметра "Текущий угол"
	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 26;

	style.update_func = [](WidgetTextField &w) {
			int hr = static_cast<int>(ctx.m_hr_monitor.getHRate());
			return hr ? std::to_string(hr) : "- - -";
		};
	addWidget(std::make_shared<WidgetTextField>(1015, 98, 141, 56, style)); // Значение параметра "Измеренное значение ЧСС"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getEnergyString();};
	addWidget(std::make_shared<WidgetTextField>(1015, 155, 141, 56, style)); // Значение параметра "Затраченная энергия"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getPowerMechString();};
	addWidget(std::make_shared<WidgetTextField>(1015, 212, 141, 56, style)); // Значение параметра "Текущая мощность"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getPowerAeroString();};
	addWidget(std::make_shared<WidgetTextField>(1015, 269, 141, 56, style)); // Значение параметра "Текущая аэробная мощность"

	style.update_func = [this](WidgetTextField &w) {return Utils::formatTime(ctx.m_session.getDuration());};
	addWidget(std::make_shared<WidgetTextField>(1015, 326, 141, 56, style)); // Значение параметра "Текущее время процедуры"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getSpeedString();};
	addWidget(std::make_shared<WidgetTextField>(1015, 440, 141, 56, style)); // Значение параметра "Текущая скорость"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getAngleString();};
	addWidget(std::make_shared<WidgetTextField>(1015, 497, 141, 56, style)); // Значение параметра "Текущий угол"
	style.update_func = UpdateFunc();

	m_total_time_text = addWidget(std::make_shared<WidgetTextField>(1015, 383, 141, 56, style, Utils::formatTime(0))); // Значение параметра "Общее время процедуры"
	style.font_size = 22;


	style.text_color_disabled = 0x434242;
	m_load_control = addWidget(std::make_shared<WidgetTextField>(0, 553, 678, 56, style, M("Управление нагрузкой")));
	style.font_style = FontStyle::Normal;
	m_speed_angle_label = addWidget(std::make_shared<WidgetTextField>(192, 717, 293, 31, style, M("СКОРОСТЬ И УГОЛ")));
	m_angle_label = addWidget(std::make_shared<WidgetTextField>(448 , 717, 219, 31, style, M("УГОЛ")));
	m_speed_label = addWidget(std::make_shared<WidgetTextField>(11, 717, 219, 31, style, M("СКОРОСТЬ")));


	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(685, 98, 414, 56, style, M("Измеренное значение ЧСС")));
	addWidget(std::make_shared<WidgetTextField>(685, 155, 414, 56, style, M("Затраченная энергия")));
	addWidget(std::make_shared<WidgetTextField>(685, 212, 414, 56, style, M("Текущая мощность")));
	addWidget(std::make_shared<WidgetTextField>(685, 269, 414, 56, style, M("Текущая аэробная мощность")));
	addWidget(std::make_shared<WidgetTextField>(685, 326, 414, 56, style, M("Текущее время процедуры")));
	addWidget(std::make_shared<WidgetTextField>(685, 383, 414, 56, style, M("Общее время процедуры")));
	addWidget(std::make_shared<WidgetTextField>(685, 440, 414, 56, style, M("Текущая скорость")));
	addWidget(std::make_shared<WidgetTextField>(685, 497, 414, 56, style, M("Текущий угол")));
	style.halign = Align::Center;
	style.font_size = 25;
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(678, 600, 149, 73, style, M("ПУСК")));
	addWidget(std::make_shared<WidgetTextField>(1141, 600, 141, 73, style, M("СТОП")));


	style.font_size = 26;
	style.text_color_passive = 0x434242;
	style.text_color_active = 0x434242;
	style.text_color_disabled = 0x737272;
	style.kbd_type = KbdType::Digital;
	style.img_file_active = "rehab_procedures_hr_text_active.png";
	style.img_file_passive = "rehab_procedures_hr_text_passive.png";
	style.img_file_disabled = "rehab_procedures_hr_text_passive.png";
	style.text_max_size = 5;

	style.min_limit = std::stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMinSpeed() / 10.));
	style.max_limit = std::stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMaxForwardSpeed() / 10.));
	style.kbd_type = KbdType::Digital | KbdType::DotFlag;
	style.num_format = ctx.m_session.getSpeedFormat();
	style.check_func = [this](std::string s, WidgetTextField &w) {
			float max_limit = std::stof(s);
			m_n_speed_text->setMaxLimit(max_limit);
			if (std::stof(m_n_speed_text->getText()) > max_limit) {
				m_n_speed_text->setText(Utils::formatFloat(ctx.m_session.getSpeedFormat(), max_limit));
			}
			onFieldChange("max_speed", max_limit);
			return "";
		};
	m_max_speed_text = addWidget(std::make_shared<WidgetTextField>(397, 155, 127, 56, style, ctx.m_session.getSpeedString(m_max_speed))); // Поле ввода параметра "Максимальная скорость"
	m_max_speed_text->setState(Widget::State::Passive);

	style.min_limit = std::stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMinSpeed() / 10.));
	style.max_limit = std::stof(m_max_speed_text->getText());
	style.check_func = CheckFunc();
	m_n_speed_text = addWidget(std::make_shared<WidgetTextField>(397, 212, 127, 56, style, ctx.m_session.getSpeedString(m_n_speed))); // Поле ввода параметра "Начальная/номинальная скорость"
	m_n_speed_text->setState(Widget::State::Passive);

	style.min_limit = 0;
	style.max_limit = std::stof(ctx.m_session.getAngleString(ctx.m_treadmill.getMaxAngle() / 100.));
	style.num_format = ctx.m_session.getAngleFormat();
	style.check_func = [this](std::string s, WidgetTextField &w) {
			float max_limit = std::stof(s);
			m_n_angle_text->setMaxLimit(max_limit);
			if (std::stof(m_n_angle_text->getText()) > max_limit) {
				m_n_angle_text->setText(Utils::formatFloat(ctx.m_session.getAngleFormat(), max_limit));
			}
			onFieldChange("max_angle", max_limit);
			return "";
		};
	m_max_angle_text = addWidget(std::make_shared<WidgetTextField>(397, 269, 127, 56, style, ctx.m_session.getAngleString(ctx.m_treadmill.getMaxAngle() / 100.))); // Поле ввода параметра "Максимальный угол"
	m_max_angle_text->setState(Widget::State::Passive);


	style.min_limit = 0;
	style.max_limit = std::stof(m_max_angle_text->getText());
	style.check_func = CheckFunc();
	m_n_angle_text = addWidget(std::make_shared<WidgetTextField>(397, 326, 127, 56, style, ctx.m_session.getAngleString(0))); // Поле ввода параметра "Начальный/номинальный угол"
	m_n_angle_text->setState(Widget::State::Passive);
	style.check_func = CheckFunc();
	style.num_format = "";

	style.min_limit = 0;
	style.max_limit = 0;
	style.kbd_type = KbdType::Digital;
	m_field_style = style;

	style.kbd_type = KbdType::None;

	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	m_warm_param = addWidget(std::make_shared<WidgetImageButton>(525, 98, "rehab_procedures_hr_param_active.png", "rehab_procedures_hr_param_passive.png", "rehab_procedures_hr_param_passive.png",
			[this](WidgetImageButton::Event e) {
				m_modal = std::make_unique<ModalWarmUpParams>();
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(525, 98, 154, 56, style, M("ПАРАМЕТРЫ"))));

	style.font_size = 26;
	style.font_style = FontStyle::Bold;
	WidgetTextFieldSP text = std::make_shared<WidgetTextField>(397, 98, 127, 56, style);
	text->setText(M("НЕТ"), "НЕТ");
	m_warm_btn = addWidget(std::make_shared<WidgetImageButton>(397, 98, "rehab_procedures_hr_text_active.png", "", "",
			[this, text](WidgetImageButton::Event e) {
				if (text->getTextId() == "НЕТ") {
					text->setText(M("ДА"), "ДА");
					m_warming_up = true;
				} else {
					text->setText(M("НЕТ"), "НЕТ");
					m_warming_up = false;
				}
				m_warm_param->setVisible(m_warming_up);
				m_warm_param->setState(m_warming_up ? Widget::State::Passive : Widget::State::Disabled);
				m_warm_time = (m_warming_up ? std::stoi(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(2)) * 60 : 0);
				update();
				return nullptr;
			}, WidgetImageButton::Type::Clicker, text));

	m_field_style.num_format = "%3.0f";
	m_field_style.check_func = [this](std::string s, WidgetTextField &w) {
			m_proc_time = std::stoi(s) * 60;
			update();
			return "";
		};
	m_field_style.min_limit = 5;
	m_field_style.max_limit = 300;
	m_proc_time_text = addWidget(std::make_shared<WidgetTextField>(397, 497, 127, 56, m_field_style, "5")); // Поле ввода параметра "Время процедуры"
	m_proc_time_text->setState(Widget::State::Passive);

	m_field_style.min_limit = 40;
	m_field_style.max_limit = std::stoi(ctx.m_db.getParam("МаксимальноДопустимаяЧСС"));
	m_field_style.check_func = [this](std::string s, WidgetTextField &w) {
			int max_limit = std::stoi(s);
			onMaxPossibleHrChange(max_limit);
			return "";
		};
	m_max_possible_hr_text = addWidget(std::make_shared<WidgetTextField>(397, 383, 127, 56, m_field_style, "120")); // Поле ввода параметра "Максимально допустимая ЧСС"
	m_max_possible_hr_text->setState(Widget::State::Passive);
	m_field_style.check_func = CheckFunc();


	m_speed_angle_btn = addWidget(std::make_shared<WidgetImageButton>(240, 615, "rehab_procedures_hr_speed_angle_active.png",
			"rehab_procedures_hr_speed_angle_disabled.png", "rehab_procedures_hr_speed_angle_passive.png",
			[this, speed_txt, angle_txt](WidgetImageButton::Event e) {
				m_speed_btn->clearTrigger();
				m_angle_btn->clearTrigger();
				speed_txt->setText(M("Начальная скорость"));
				angle_txt->setText(M("Начальный угол"));
				return nullptr;
			}, WidgetImageButton::Type::Trigger));

	m_angle_btn = addWidget(std::make_shared<WidgetImageButton>(507, 615, "rehab_procedures_hr_angle_active.png",
			"rehab_procedures_hr_angle_disabled.png", "rehab_procedures_hr_angle_passive.png",
			[this, speed_txt, angle_txt](WidgetImageButton::Event e) {
				m_speed_btn->clearTrigger();
				if (m_mode == Mode::MinMax) {
					m_speed_angle_btn->clearTrigger();
				}
				speed_txt->setText(M("Номинальная скорость"));
				angle_txt->setText(M("Начальный угол"));
				return nullptr;
			}, WidgetImageButton::Type::Trigger));

	m_speed_btn = addWidget(std::make_shared<WidgetImageButton>(70, 615, "rehab_procedures_hr_speed_active.png",
			"rehab_procedures_hr_speed_disabled.png", "rehab_procedures_hr_speed_passive.png",
			[this, speed_txt, angle_txt](WidgetImageButton::Event e) {
				if (m_mode == Mode::MinMax) {
					m_speed_angle_btn->clearTrigger();
				}
				m_angle_btn->clearTrigger();
				speed_txt->setText(M("Начальная скорость"));
				angle_txt->setText(M("Номинальный угол"));
				return nullptr;
			}, WidgetImageButton::Type::Trigger));
	m_speed_btn->setTrigger();

	m_start_btn = addWidget(std::make_shared<WidgetImageButton>(826, 570, "rehab_test_protocol_start.png", "", "",
			[this, name_in_history](WidgetImageButton::Event e) {
				if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
					m_paused = true;
					ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
					m_start_btn->setPassive("rehab_test_protocol_pause.png");
					m_start_btn->setActive("rehab_test_protocol_pause.png");
					m_timer.stopTimer();
				} else {
					if (m_paused) {
						m_paused = false;
					} else {

						if (!ctx.m_hr_monitor.getHRate()) {
							using Flags = ModalCommonDialog::Flags;
							m_modal = std::make_unique<ModalCommonDialog>(M("НЕТ ДАННЫХ С ДАТЧИКА ЧСС"),
									M("ПОДКЛЮЧИТЕ ДАТЧИК ЧСС"), "HRE1", Flags::YellowTitle | Flags::CloseButton,
									"icon_hr_exclamation_yellow.png");
							return nullptr;
						}


						ctx.m_session.start(ctx.m_patient_id, name_in_history);
						m_interval_cnt = getBosCorrectionData(0).interval;
						m_first_interval = true;
						if (m_warming_up) {
							ctx.m_treadmill.setSpeed(m_warm_start_speed * 10);
							ctx.m_treadmill.setAngle(0);
						} else {
							ctx.m_treadmill.setSpeed(ctx.m_session.getSpeedKmH(stof(m_n_speed_text->getText())) * 10);
							ctx.m_treadmill.setAngle(ctx.m_session.getAngleDegree(stof(m_n_angle_text->getText())) * 100);
						}
					}
					ctx.m_treadmill.setState(TreadmillDriver::RunState::Run);
					onStateChange(Widget::State::Disabled);
					m_start_btn->setPassive("rehab_test_protocol_start.png");
					m_start_btn->setActive("rehab_test_protocol_start.png");
					m_warm_btn->setState(Widget::State::Disabled);
					m_max_possible_hr_text->setState(Widget::State::Disabled);
					m_max_speed_text->setState(Widget::State::Disabled);
					m_n_speed_text->setState(Widget::State::Disabled);
					m_max_angle_text->setState(Widget::State::Disabled);
					m_n_angle_text->setState(Widget::State::Disabled);
					m_proc_time_text->setState(Widget::State::Disabled);
					m_warm_param->setState(Widget::State::Disabled);
					if (!m_speed_angle_btn->isSet()) {
						m_speed_angle_btn->setState(Widget::State::Disabled);
					}
					if (!m_speed_btn->isSet()) {
						m_speed_btn->setState(Widget::State::Disabled);
					}
					if (!m_angle_btn->isSet()) {
						m_angle_btn->setState(Widget::State::Disabled);
					}
					m_timer.startTimerMillis(1000);
				}
				m_stop_btn->clearTrigger();
				return nullptr;
		}));

	m_stop_btn = addWidget(std::make_shared<WidgetImageButton>(984, 570, "rehab_test_protocol_stop.png", "", "",
			[this](WidgetImageButton::Event e) {
				stopAction();
				return nullptr;
		}, WidgetImageButton::Type::Trigger));

	m_stop_btn->setTrigger();
	m_warm_time = (m_warming_up ? std::stoi(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(2)) * 60 : 0);
	m_warm_param->setVisible(m_warming_up);
	m_warm_param->setState(m_warming_up ? Widget::State::Passive : Widget::State::Disabled);

	m_proc_time = stoi(m_proc_time_text->getText()) * 60;
	update();
}

RehabBosBaseMenu::~RehabBosBaseMenu() {
	m_timer.stopTimer();
	ctx.m_session.finish();
	ctx.m_treadmill.doZeroPoint();
	LD("Dtor");
}

Menu* RehabBosBaseMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "run") {

		ctx.m_session.incSec();
		onTick();

		if (ctx.m_session.getHrate() >= std::stoi(m_max_possible_hr_text->getText())) {
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::make_unique<ModalCommonDialog>(M("ПРЕВЫШЕНО МАКСИМАЛЬНО\\ДОПУСТИМОЕ ЗНАЧЕНИЕ ЧСС"),
					M("БЕГ ОСТАНОВЛЕН"), "HRE2", Flags::YellowTitle | Flags::CloseButton,
					"icon_hr_exclamation_yellow.png");
			stopAction();
			return ret;
		}

		if (!ctx.m_session.getHrate() && !m_modal && !m_stop_btn->isSet()) {
			m_msg_counter = 60;
			using Flags = ModalCommonDialog::Flags;
			std::unique_ptr<ModalCommonDialog> no_hr_modal = std::make_unique<ModalCommonDialog>(M("НЕТ ДАННЫХ С ДАТЧИКА ЧСС"),
					M("ОСТАНОВКА ЧЕРЕЗ ") + std::to_string(m_msg_counter) + M(" СЕКУНД\\ОСТАНОВИТЬ СЕЙЧАС?"), "HRE3", Flags::YellowTitle | Flags::Yes,
					"icon_hr_exclamation_yellow.png");
			no_hr_modal->setTimer(1000, [this](int id) {
					if (--m_msg_counter <= 0) {
						stopAction();
						ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					} else if (m_msg_counter < 30 && ctx.m_treadmill.getSpeed() > 20) {
						ctx.m_treadmill.setSpeed(20);
					}
					if (ctx.m_session.getHrate()) {
						ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					}
					static_cast<ModalCommonDialog*>(m_modal.get())->setText(M("ОСТАНОВКА ЧЕРЕЗ ") + std::to_string(m_msg_counter) + M(" СЕКУНД\\ОСТАНОВИТЬ СЕЙЧАС?"));
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
				});

			m_modal = std::move(no_hr_modal);

			return ret;
		}

		if (m_warming_up && ctx.m_session.getDuration() <= m_warm_time) {
			LD("WarmUp set speed: " + std::to_string(m_warm_start_speed + m_warm_koef * ctx.m_session.getDuration())
				+ "   m_warm_koef:" + std::to_string(m_warm_koef) + "    ctx.m_session.getDuration(): " + std::to_string(ctx.m_session.getDuration()));
			ctx.m_treadmill.setSpeed((m_warm_start_speed + m_warm_koef * ctx.m_session.getDuration()) * 10);
		} else if (ctx.m_session.getDuration() >= m_total_time) {
			stopAction();
		} else if (m_first_interval) {
			m_first_interval = false;
			ctx.m_treadmill.setSpeed(ctx.m_session.getSpeedKmH(stof(m_n_speed_text->getText())) * 10);
			ctx.m_treadmill.setAngle(ctx.m_session.getAngleDegree(stof(m_n_angle_text->getText())) * 100);
		} else {
			if (m_interval_cnt > 1) {
				m_interval_cnt--;
			} else {
				bosCorrection(getBosCorrectionDelta());
			}
		}

		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}



	if (m_modal && m_modal->getType() == "HRENOPAT"	&& event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		return  new RehabSelHrMenu();
	} else if (m_modal && m_modal->getType().substr(0, 3) == "HRE"
			&& (event->getType() == Event::EventType::ModalYes || event->getType() == Event::EventType::ModalCancel)) {
		m_modal.reset();
		if (event->getType() == Event::EventType::ModalYes) {
			stopAction();
		}
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	if (m_modal && m_modal->getType().substr(0, 3) == "WUP"	&&  event->getType() == Event::EventType::ModalCancel) {
		m_modal.reset();
		m_warm_start_speed = ctx.m_session.getSpeedKmH(stof(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(0)));
		m_warm_end_speed = ctx.m_session.getSpeedKmH(stof(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(1)));
		m_warm_time = stoi(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(2)) * 60;
		update();
	}

	TreadmillDriver::RunState state = ctx.m_treadmill.getState();
	if (m_cur_state != state) {
		using  RState = TreadmillDriver::RunState;

		if (state == RState::Pause) {
			m_paused = true;
			ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
			m_start_btn->setPassive("rehab_test_protocol_pause.png");
			m_start_btn->setActive("rehab_test_protocol_pause.png");
			m_timer.stopTimer();
		} else if (state == RState::Stop) {
			stopAction();
		}

		m_cur_state = state;
	}

	ret = Menu::processEvent(std::move(event));
	if (m_event != EventType::None) {
		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
			if (m_event == EventType::Stop) {
				stopAction();
			} else if (m_event == EventType::Pause) {
				m_paused = true;
				ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
				m_start_btn->setPassive("rehab_test_protocol_pause.png");
				m_start_btn->setActive("rehab_test_protocol_pause.png");
				m_timer.stopTimer();
			}

		}
	}

	return ret;
}

void RehabBosBaseMenu::update() {
	LD("Update in base " + std::to_string(m_warm_time));
	m_total_time = m_warm_time + m_proc_time;
	m_total_time_text->setText(Utils::formatTime(m_total_time));
	m_warm_koef = (m_warm_end_speed - m_warm_start_speed) / m_warm_time;
}

void RehabBosBaseMenu::stopAction() {
	m_paused = false;
	ctx.m_session.finish();
	ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
	ctx.m_treadmill.setAngle(0);
	m_timer.stopTimer();
	onStateChange(Widget::State::Passive);
	m_speed_angle_btn->setState(Widget::State::Passive);
	m_speed_btn->setState(Widget::State::Passive);
	m_angle_btn->setState(Widget::State::Passive);
	m_warm_btn->setState(Widget::State::Passive);
	m_max_possible_hr_text->setState(Widget::State::Passive);
	m_max_speed_text->setState(Widget::State::Passive);
	m_n_speed_text->setState(Widget::State::Passive);
	m_max_angle_text->setState(Widget::State::Passive);
	m_n_angle_text->setState(Widget::State::Passive);
	m_proc_time_text->setState(Widget::State::Passive);
	m_warm_param->setState(m_warming_up ? Widget::State::Passive : Widget::State::Disabled);

	m_start_btn->setPassive("");
	m_start_btn->setActive("rehab_test_protocol_start.png");
	m_stop_btn->setTrigger();
}


void RehabBosBaseMenu::bosCorrection(int delta) {
	BosCorrectionData bos = getBosCorrectionData(delta);
	m_interval_cnt = bos.interval;

	if (delta < 0) {
		bool try_angle = false;
		if (!m_angle_btn->isSet()) {
			int speed = ctx.m_treadmill.getSpeed() + bos.speed;
			if (speed > ctx.m_treadmill.getMaxForwardSpeed()) {
				speed = ctx.m_treadmill.getMaxForwardSpeed();
				if (m_speed_angle_btn->isSet()) {
					try_angle = true;
				}
			}
			ctx.m_treadmill.setSpeed(speed);
		}
		if (m_angle_btn->isSet() || try_angle) {
			int angle = ctx.m_treadmill.getAngle() + bos.angle;
			if (angle > ctx.m_treadmill.getMaxAngle()) {
				angle = ctx.m_treadmill.getMaxAngle();
			}
			ctx.m_treadmill.setAngle(angle);
		}
	} else if (delta > 0) {
		bool try_speed = false;
		if (!m_speed_btn->isSet()) {
			int angle = ctx.m_treadmill.getAngle() - bos.angle;
			if (angle < 0) {
				angle = 0;
				if (m_speed_angle_btn->isSet()) {
					try_speed = true;
				}
			}
			ctx.m_treadmill.setAngle(angle);
		}
		if (m_speed_btn->isSet() || try_speed) {
			int speed = ctx.m_treadmill.getSpeed() - bos.speed;
			if (speed < ctx.m_treadmill.getMinSpeed()) {
				speed = ctx.m_treadmill.getMinSpeed();
			}
			ctx.m_treadmill.setSpeed(speed);
		}
	}
}
