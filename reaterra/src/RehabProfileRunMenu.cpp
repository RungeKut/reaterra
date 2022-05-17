#include "RehabProfileRunMenu.h"

#include <cmath>
#include <vector>

#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
#include "RehabSelProfileMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetProfil.h"

RehabProfileRunMenu::RehabProfileRunMenu()
:	Menu {0, 0, "rehab_profile_run_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_paused {false},
	m_total_time {0},
	m_current_interval {0},
	m_interval_time {0},
	m_prev_speed {0},
	m_cur_state {ctx.m_treadmill.getState()}
{
	LD("Ctor");
	int id = stoi(ctx.m_prev_menu["RehabProfileRunMenu.params"].at(0));
	m_intervals = ctx.m_db.getIntervals(id);
	m_params = ctx.m_db.getProtocolParams(id);
	m_intervals_data = ctx.m_db.getIntervals(id);
	LD("id = " + std::to_string(id));
	ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
	ctx.m_treadmill.setAngle(0);

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("ПРОФИЛИ"), [this](WidgetImageButton::Event e){
				ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Profil))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsMenu>());
			}, bc_link_style},
		{M("бег по профилю"), WIBFunc(), bc_last_style}
	});

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = 0x333333;
	style.font_size = 21;
	style.font_style = FontStyle::Bold;
	style.lines = 2;

	std::string &name = m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name));
	std::string &description = m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Description));
	addWidget(std::make_shared<WidgetTextField>(0, 47, 227, 52, style, name));
	addWidget(std::make_shared<WidgetTextField>(227, 47, 1053, 52, style, description));

	style.font_style = FontStyle::Normal;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(0, 159, 226, 43, style, M("СКОРОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(0, 375, 226, 43, style, M("УГОЛ ПОДЪЕМА")));
	addWidget(std::make_shared<WidgetTextField>(0, 254, 227, 40, style, M(ctx.m_db.getParam("ЕдиницыСкорость"))));
	addWidget(std::make_shared<WidgetTextField>(0, 470, 227, 40, style, M(ctx.m_db.getParam("ЕдиницыУгол"))));
	style.text_color_disabled = 0x333333;
	addWidget(std::make_shared<WidgetTextField>(0, 516, 382, 32, style, M("СУММАРНОЕ ВРЕМЯ ПРОФИЛЯ")));
	addWidget(std::make_shared<WidgetTextField>(73, 631, 309, 32, style, M("ВРЕМЯ БЕГА")));
	addWidget(std::make_shared<WidgetTextField>(383, 516, 541, 32, style, M("МОЩНОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(382, 631, 198, 32, style, M("РАССТОЯНИЕ")));
	addWidget(std::make_shared<WidgetTextField>(581, 631, 181, 32, style, M("ЭНЕРГИЯ")));
	addWidget(std::make_shared<WidgetTextField>(763, 631, 161, 32, style, M("ПУЛЬС")));
	style.halign = Align::Right;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(924, 526, 168, 46, style, M("ПУСК")));
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(1112, 526, 168, 46, style, M("СТОП")));

	style.halign = Align::Center;
	style.text_color_disabled = 0x333333;
	addWidget(std::make_shared<WidgetTextField>(73, 599, 309, 32, style, M("ч:мин:с")));
	addWidget(std::make_shared<WidgetTextField>(73, 714, 309, 32, style, M("ч:мин:с")));
	addWidget(std::make_shared<WidgetTextField>(383, 599, 270, 32, style, M("механическая, ") + M(ctx.m_db.getParam("ЕдиницыМощность"))));
	addWidget(std::make_shared<WidgetTextField>(654, 599, 270, 32, style, M("аэробная, ") + M(ctx.m_db.getParam("ЕдиницыМощностьАэробная"))));
	addWidget(std::make_shared<WidgetTextField>(415, 714, 165, 32, style, M(ctx.m_db.getParam("ЕдиницыРасстояние"))));
	addWidget(std::make_shared<WidgetTextField>(581, 714, 181, 32, style, M(ctx.m_db.getParam("ЕдиницыЭнергия"))));
	addWidget(std::make_shared<WidgetTextField>(763, 714, 161, 32, style, M("удар/мин")));


	style.font_style = FontStyle::Bold;
	style.font_size = 32;
	style.update_func = [](WidgetTextField &w) {
			int hr = static_cast<int>(ctx.m_session.getHrate());
			return hr ? std::to_string(hr) : "- - -";
		};
	addWidget(std::make_shared<WidgetTextField>(809, 663, 115, 52, style)); // Значение параметра "Измеренное значение ЧСС"

	style.update_func = [this](WidgetTextField &w) {
		std::string str = ctx.m_session.getEnergyString();
		w.setFontSize(str.size() > 6 ? 28 : 32);
		return str;
	};
	addWidget(std::make_shared<WidgetTextField>(614, 663, 148, 52, style)); // Значение параметра "Затраченная энергия"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getPowerMechString();};
	addWidget(std::make_shared<WidgetTextField>(416, 548, 237, 52, style)); // Значение параметра "Текущая мощность"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getPowerAeroString();};
	addWidget(std::make_shared<WidgetTextField>(654, 548, 270, 52, style)); // Значение параметра "Текущая аэробная мощность"

	style.text_color_disabled = 0xCC9933;
	style.update_func = [this](WidgetTextField &w) {return Utils::formatTime(ctx.m_session.getDuration());};
	addWidget(std::make_shared<WidgetTextField>(73, 663, 309, 52, style)); // Значение параметра "Текущее время процедуры"

	style.text_color_disabled = 0x333333;
	style.text_color_disabled = to_base(Colors::White);
	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getSpeedString();};
	addWidget(std::make_shared<WidgetTextField>(0, 201, 227, 53, style)); // Значение параметра "Текущая скорость"

	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getAngleString();};
	addWidget(std::make_shared<WidgetTextField>(0, 417, 227, 53, style)); // Значение параметра "Текущий угол"

	style.text_color_disabled = 0x333333;
	style.update_func = [this](WidgetTextField &w) {return ctx.m_session.getDistanceString();};
	addWidget(std::make_shared<WidgetTextField>(383, 663, 197, 52, style)); // Значение параметра "Текущее расстояние"

	style.update_func = UpdateFunc();

	m_duration = 0;
	float speed1 = 0;
	float accel;
	std::vector<std::tuple<unsigned, float, float>> points;
	points.push_back({0, 0, 0});
	for (const auto &il : m_intervals_data) {
		float speed2 = std::stof(il.at(static_cast<int>(DbConnector::IntervalData::Speed)));
		accel = ctx.m_treadmill.getAccelValue(std::stof(il.at(to_base(DbConnector::IntervalData::Acceleration)))) / 1000.;
		unsigned dur = std::stoi(il.at(static_cast<int>(DbConnector::IntervalData::Duration)));
		unsigned accel_time = getAccelTime(speed1, speed2, accel);
		m_duration += accel_time;
		points.push_back({m_duration, speed2, std::stof(il.at(static_cast<int>(DbConnector::IntervalData::Angle)))});
		m_duration += dur;
		points.push_back({m_duration, speed2, std::stof(il.at(static_cast<int>(DbConnector::IntervalData::Angle)))});
		speed1 = speed2;
	}

	m_duration += getAccelTime(speed1, 0, ctx.m_treadmill.getAccelValue(ctx.m_decel) / 1000.);

//	LD("accel:" + std::to_string(accel) + "  dur:" + std::to_string(getAccelTime(speed1, 0, accel)));

	points.push_back({m_duration, 0, 0});

	for (auto &p : points) {
		LD("x: " + std::to_string(std::get<0>(p)) + "  s: " + std::to_string(std::get<1>(p)) + "  a: " + std::to_string(std::get<2>(p)));
	}

	addWidget(std::make_shared<WidgetTextField>(73, 548, 309, 52, style, Utils::formatTime(m_duration))); // Значение параметра "Общее время процедуры"



	addWidget(std::make_shared<WidgetProfil>(226, 99, points));

	style.text_color_disabled = to_base(Colors::White);
	style.font_style = FontStyle::Normal;
	style.font_size = 22;
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(226, 278, 30, 33, style, "0")); // Ноль на графике скорости
	addWidget(std::make_shared<WidgetTextField>(226, 486, 30, 33, style, "0")); // Ноль на графике угла
	addWidget(std::make_shared<WidgetTextField>(226, 96, 100, 33, style, ctx.m_session.getSpeedString(ctx.m_treadmill.getMaxForwardSpeed() / 10.))); // Максимум на графике скорости
	addWidget(std::make_shared<WidgetTextField>(226, 305, 100, 33, style, ctx.m_session.getAngleString(ctx.m_treadmill.getMaxAngle() / 100.))); // Максимум на графике угла

	addWidget(std::make_shared<WidgetImageButton>(1237, 313, "", "rehab_profile_run_menu_sandglass.png"))->setState(Widget::State::Disabled);

	m_start_btn = addWidget(std::make_shared<WidgetImageButton>(945, 570, "rehab_test_protocol_start.png", "", "",
			[this](WidgetImageButton::Event e) {
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
						ctx.m_session.start(ctx.m_patient_id, M("Польз. профиль"));
						ctx.m_treadmill.setSpeed(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Speed))) * 10);
						ctx.m_treadmill.setAngle(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Angle))) * 100);
						ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(std::stoi(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Acceleration)))) * 10);
						ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(std::stoi(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Acceleration)))) * 10);
					}
					ctx.m_treadmill.setState(TreadmillDriver::RunState::Run);
					m_start_btn->setPassive("rehab_test_protocol_start.png");
					m_start_btn->setActive("rehab_test_protocol_start.png");
					m_timer.startTimerMillis(1000);
				}
				m_stop_btn->clearTrigger();
				return nullptr;
		}));

	m_stop_btn = addWidget(std::make_shared<WidgetImageButton>(1103, 570, "rehab_test_protocol_stop.png", "", "",
			[this](WidgetImageButton::Event e) {
				stopAction();
				return nullptr;
		}, WidgetImageButton::Type::Trigger));

	m_stop_btn->setTrigger();
}

RehabProfileRunMenu::~RehabProfileRunMenu() {
	m_timer.stopTimer();
	ctx.m_session.finish();
	ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
	ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
	ctx.m_treadmill.doZeroPoint();
	LD("Dtor");
}

Menu* RehabProfileRunMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "run") {

		m_interval_time++;
		ctx.m_session.incSec();

		if (m_interval_time >= std::stoi(m_intervals_data.at(m_current_interval).at(to_base(DbConnector::IntervalData::Duration)))
					+ getAccelTime(m_prev_speed, std::stof(m_intervals_data.at(m_current_interval).at(to_base(DbConnector::IntervalData::Speed))),
							ctx.m_treadmill.getAccelValue(std::stof(m_intervals_data.at(m_current_interval).at(to_base(DbConnector::IntervalData::Acceleration)))) / 1000.))
		{
			if (m_current_interval < m_intervals_data.size() - 1) {
				m_prev_speed = std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Speed)));
				m_current_interval++;
				m_interval_time = 0;
				float speed = std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Speed)));
				ctx.m_treadmill.setSpeed(speed * 10);
				ctx.m_treadmill.setAngle(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Angle))) * 100);
				ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(std::stoi(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Acceleration)))) * 10);
				ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(std::stoi(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Acceleration)))) * 10);
			} else {
				if (ctx.m_treadmill.getSpeed() != 0 ) {
					ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
					ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
					ctx.m_treadmill.setAngle(0);
					ctx.m_treadmill.setSpeed(1);
				}
				if (static_cast<unsigned>(ctx.m_session.getDuration()) >= m_duration) {
					stopAction();
				}
				return ret;
			}
		}
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	TreadmillDriver::RunState state = ctx.m_treadmill.getState();
	if (m_cur_state != state) {
		using  RState = TreadmillDriver::RunState;

		if (state == RState::Pause) {
			m_paused = true;
//			ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
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
//				ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
				m_start_btn->setPassive("rehab_test_protocol_pause.png");
				m_start_btn->setActive("rehab_test_protocol_pause.png");
				m_timer.stopTimer();
			}

		}
	}
	return ret;
}

int RehabProfileRunMenu::getAccelTime(float speed1, float speed2, float accel) {
	float sp1 = Utils::physConvert(Utils::Phys::Speed, Utils::Unit::KmPerHour, Utils::Unit::MPerSec, speed1);
	float sp2 = Utils::physConvert(Utils::Phys::Speed, Utils::Unit::KmPerHour, Utils::Unit::MPerSec, speed2);

	LD("time: " + std::to_string(fabs((sp1 - sp2) / accel)) + "sp1: " + std::to_string(sp1) + " (" + std::to_string(speed1) + ")   sp2: " + std::to_string(sp2) + " (" + std::to_string(speed2) + ")   accel: " + std::to_string(accel));
	return fabs((sp1 - sp2) / accel);
}

void RehabProfileRunMenu::stopAction() {
	m_paused = false;
	ctx.m_session.finish();
	ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
	m_timer.stopTimer();
	ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
	ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);

	m_current_interval = 0;
	m_interval_time = 0;
	m_prev_speed = 0;
	m_start_btn->setPassive("");
	m_start_btn->setActive("rehab_test_protocol_start.png");
	m_stop_btn->setTrigger();
}
