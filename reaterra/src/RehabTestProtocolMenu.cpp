#include "RehabTestProtocolMenu.h"

#include <cmath>

#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalListSelector.h"
#include "RehabMenu.h"
#include "RehabProtocolSelectionMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetRectangle.h"
#include "Utils.h"

RehabTestProtocolMenu::RehabTestProtocolMenu()
:	Menu {0, 0, "rehab_test_protocol.png"},
	m_time {0},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer));}, 0},
	m_refresh_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));}, 1},
	m_rows_start {177},
	m_row_step {38},
	m_current_interval {0},
	m_interval_time {0},
	m_not_count {false},
	m_cur_state {ctx.m_treadmill.getState()}
{
	LD("Ctor");
	int id = stoi(ctx.m_prev_menu["RehabTestProtocolMenu.params"].at(0));
	m_modifs_list = ctx.m_db.getProtocolModifs(id);
	m_params = ctx.m_db.getProtocolParams(id);
	LD("ProtoId: " + std::to_string(id));

	if (m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Type)) == "USER") {
		m_proto_name = M("Польз. протокол");
	} else {
		m_proto_name = m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name));
	}

	WTFStyle bc_style {bc_last_style};
	std::string title;
	if (m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name)) == "Gardner") {
		title = M("тестирование по ангиологическому протоколу Gardner");
		bc_style.font_size = 22;
	} else {
		title = M("тестирование по протоколу ") + m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name));
	}

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{title, WIBFunc(), bc_style}
	});

	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(0, 46, 699, 36, style, M("ТЕСТИРОВАНИЕ ПО ПРОТОКОЛУ")));
	style.font_size = 22;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0xD1E6FF;
	addWidget(std::make_shared<WidgetTextField>(0, 118, 142, 39, style, M("СТУПЕНЬ")));
	addWidget(std::make_shared<WidgetTextField>(142, 118, 216, 39, style, M("ДЛИТЕЛЬНОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(358, 118, 170, 39, style, M("СКОРОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(528, 118, 170, 39, style, M("УГОЛ")));
	style.halign = Align::Left;
	style.font_size = 24;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(779, 185, 236, 46, style, M("ВРЕМЯ БЕГА")));
	addWidget(std::make_shared<WidgetTextField>(779, 221, 236, 46, style, M("час:мин:сек")));
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(803, 270, 373, 46, style, M("МОЩНОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(772, 412, 114, 46, style, M("ПУЛЬС")));
	addWidget(std::make_shared<WidgetTextField>(913, 412, 367, 46, style, M("МАКСИМАЛЬНЫЙ ПУЛЬС")));
	addWidget(std::make_shared<WidgetTextField>(770, 524, 147, 35, style, M("уд/мин")));
	addWidget(std::make_shared<WidgetTextField>(1084, 524, 147, 35, style, M("уд/мин")));
	style.text_color_disabled = to_base(Colors::White);
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(698, 616, 138, 43, style, M("ПУСК")));
	addWidget(std::make_shared<WidgetTextField>(1141, 616, 138, 43, style, M("СТОП")));


	int xs[] = {0, 142, 359, 528, 699};


	int x[] = {71, 250, 444, 613};

	m_duration_unit = std::make_shared<WidgetText>(x[1], m_rows_start - 6, "", Align::Center, 24, 0xD1E6FF, 0xD1E6FF);
	m_speed_unit = std::make_shared<WidgetText>(x[2], m_rows_start - 6, "", Align::Center, 24, 0xD1E6FF, 0xD1E6FF);
	m_angle_unit = std::make_shared<WidgetText>(x[3], m_rows_start - 6, "", Align::Center, 24, 0xD1E6FF, 0xD1E6FF);

	addWidget(m_duration_unit);
	addWidget(m_speed_unit);
	addWidget(m_angle_unit);

	for (int i = 0; i < TABLE_COLS; ++i) {
		m_high_light.push_back(std::make_shared<WidgetRectangle>(xs[i], m_rows_start, xs[i + 1] - xs[i] - 1, m_row_step - 1, 0x3BB9EB));
		addWidget(m_high_light.back());
	}

	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		m_table.push_back({});
		for (unsigned j = 0; j < TABLE_COLS; ++j) {
			m_table.back().push_back(std::make_shared<WidgetText>(x[j], m_rows_start + m_row_step * i + 28, "", Align::Center, 25, 0xFFFFFF, 0xFFFFFF));
			addWidget(m_table.back().back());
		}
	}

	m_modif_name = std::make_shared<WidgetText>(955, 138, "", Align::Center, 24, 0xFFFFFF, 0xFFFFFF,
			std::function<std::string(void)>{}, FontStyle::Bold);

	updateData(id);

	std::string unit {M("механическая, ") + M(ctx.m_db.getParam("ЕдиницыМощность"))};
	addWidget(std::make_shared<WidgetText>(844, 407, unit, Align::Center, 24, 0x434242, 0x434242));
	uint32_t color = 0x3C3C3C;
	if (ctx.m_patient.empty()) {
		color = DEC2RGB(190, 190, 200);
	}
	addWidget(std::make_shared<WidgetText>(867, 367, "", Align::Center, 55, DEC2RGB(90, 90, 90), color,
			[this]() {
				return ctx.m_session.getPowerMechString();
			}));

	std::string un = ctx.m_db.getParam("ЕдиницыМощностьАэробная");
	unit = M("аэробная") + "," + (un == "MET" ? " " : "") + M(un);

	addWidget(std::make_shared<WidgetText>(1135, 407, unit, Align::Center, 24, 0x434242, 0x434242));
	addWidget(std::make_shared<WidgetText>(1135, 367, "", Align::Center, 55, DEC2RGB(90, 90, 90), color,
			[this]() {
				return ctx.m_session.getPowerAeroString();
			}));


	addWidget(std::make_shared<WidgetText>(1125, 244, "", Align::Center, 55, DEC2RGB(90, 90, 90), 0x3C3C3C,
			[this]() {
				return Utils::formatTime(ctx.m_session.getDuration());
			}));



	addWidget(std::make_shared<WidgetText>(845, 512, "", Align::Center, 55, 0x3C3C3C, 0x3C3C3C,
			[this]() {
				int hr = static_cast<int>(ctx.m_session.getHrate());
				return hr ? std::to_string(hr) : "- - -";
			}));

	addWidget(std::make_shared<WidgetText>(1157, 512, "", Align::Center, 55, 0x3C3C3C, 0x3C3C3C,
			[this]() {
				int hr = static_cast<int>(ctx.m_session.getHrateMax());
				return hr ? std::to_string(hr) : "- - -";
			}));


	m_start = std::make_shared<WidgetImageButton>(836, 570, "rehab_test_protocol_start.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_running = true;
				if (m_not_count) {
					m_msg_line_1->setText("");
					m_msg_line_2->setText("");
					m_msg_line_3->setText("");
					m_current_interval = 1;
					m_interval_time = 0;
					m_start->setPassive("rehab_test_protocol_start.png");
					m_start->setActive("rehab_test_protocol_start.png");
					m_not_count = false;
					highLightRow(m_current_interval);
					return nullptr;
				}
				if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
					m_running = false;
					ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
					m_start->setPassive("rehab_test_protocol_pause.png");
					m_start->setActive("rehab_test_protocol_pause.png");
					m_timer.stopTimer();
				} else {
					if (m_time == 0 && !m_not_count) {
						highLightRow(0);

						if (m_intervals_data.at(0).at(static_cast<int>(DbConnector::IntervalData::Type)) == "1") {
							m_msg_line_1->setText(M("ПОВТОРНО НАЖМИТЕ ПУСК"));
							m_msg_line_2->setText(M("КОГДА ПАЦИЕНТ НАСТУПИТ"));
							m_msg_line_3->setText(M("НА БЕГОВОЕ ПОЛОТНО"));
							m_not_count = true;
						}

						if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Stop) {
							ctx.m_session.start(ctx.m_patient_id, m_proto_name + (m_modif_name->getText().empty() ? "" : " " + M(m_modif_name->getText())));
						}
						ctx.m_treadmill.setSpeed(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Speed))) * 10);
						ctx.m_treadmill.setAngle(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Angle))) * 100);
					}
					ctx.m_treadmill.setState(TreadmillDriver::RunState::Run);
					if (!m_not_count) {
						m_start->setPassive("rehab_test_protocol_start.png");
						m_start->setActive("rehab_test_protocol_start.png");
					}
					m_timer.startTimerMillis(1000);
				}
				m_stop->setPassive("");
				if (m_modif) {
					m_modif->setState(Widget::State::Disabled);
				}
				return nullptr;
		});
	addWidget(m_start);

	m_stop = std::make_shared<WidgetImageButton>(994, 570, "rehab_test_protocol_stop.png", "", "rehab_test_protocol_stop.png",
			[this](WidgetImageButton::Event e) {
				stopAction();
				return nullptr;
		});
	addWidget(m_stop);

	m_msg_line_1 = std::make_shared<WidgetText>(989, 80, "", Align::Center, 24, 0x003264, 0x003264,
			std::function<std::string(void)>{}, FontStyle::Bold);
	m_msg_line_2 = std::make_shared<WidgetText>(989, 120, "", Align::Center, 24, 0x003264, 0x003264,
			std::function<std::string(void)>{}, FontStyle::Bold);
	m_msg_line_3 = std::make_shared<WidgetText>(989, 160, "", Align::Center, 24, 0x003264, 0x003264,
			std::function<std::string(void)>{}, FontStyle::Bold);
	addWidget(m_msg_line_1);
	addWidget(m_msg_line_2);
	addWidget(m_msg_line_3);

	if (m_modifs_list.size() > 1) {
		m_msg_line_1->setText(M("ВЫБОР МОДИФИКАЦИИ ПРОТОКОЛА"));
		m_modif = std::make_shared<WidgetImageButton>(747, 93, "rehab_test_protocol_modif_active.png", "rehab_test_protocol_modif_disable.png", "rehab_test_protocol_modif.png",
				[this](WidgetImageButton::Event e) {
				m_modal = std::unique_ptr<Menu>(new ModalListSelector(699, 177, m_modifs_list, "rehab_test_protocol_modif_selector_active.png", "rehab_test_protocol_modif_selector_passive.png"));
				return nullptr;
			});
		addWidget(m_modif);
	}
	addWidget(m_modif_name);

	if (m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Type)) == "USER") {
		addWidget(std::make_shared<WidgetImageButton>(699, 46, "", "rehab_test_protocol_description.png"))->setState(Widget::State::Disabled);

		WTFStyle style {default_style};
		style.kbd_type = KbdType::None;
		style.rect_color_active = to_base(Colors::None);
		style.halign = Align::Center;
		style.text_color_disabled = to_base(Colors::BlueOnYellow);
		style.font_size = 22;
		style.font_style = FontStyle::Bold;

		addWidget(std::make_shared<WidgetTextField>(699, 46, 581, 41, style, M("ОПИСАНИЕ ПРОТОКОЛА")));


		addWidget(std::make_shared<WidgetText>(990, 132, m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Description)), Align::Center, 24, 0x434242, 0x434242,
				std::function<std::string(void)>{}, FontStyle::Normal, 581, 3));
	}

	m_refresh_timer.startTimerMillis(200);
}

RehabTestProtocolMenu::~RehabTestProtocolMenu() {
	m_refresh_timer.stopTimer();
	m_timer.stopTimer();
	ctx.m_session.finish();
	ctx.m_treadmill.doZeroPoint();
	LD("Dtor");
}

Menu* RehabTestProtocolMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer) {
		if (!m_not_count) {
			m_time++;
			ctx.m_session.incSec();
		}
		m_interval_time++;

		if (m_interval_time >= std::stoi(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Duration)))) {
			if (m_current_interval < m_intervals_data.size() - 1) {
				if (m_not_count) {
					m_start->setPassive("rehab_test_protocol_start.png");
					m_start->setActive("rehab_test_protocol_start.png");
					m_not_count = false;
				}
				m_current_interval++;
				highLightRow(m_current_interval);
				m_interval_time = 0;
				if (m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Type)) == "0"
						&& m_modifs_list.size() < 2) {
					m_msg_line_1->setText("");
					m_msg_line_2->setText("");
					m_msg_line_3->setText("");
				}
				ctx.m_treadmill.setSpeed(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Speed))) * 10);
				ctx.m_treadmill.setAngle(std::stof(m_intervals_data.at(m_current_interval).at(static_cast<int>(DbConnector::IntervalData::Angle))) * 100);
			} else {
				stopAction();
				return ret;
			}
		}
	}


	std::string extra = event->getExtra();
	if (event->getType() == Event::EventType::ModalYes && extra.substr(0, 5) == "MODIF") {
		m_modal.reset();
		LD("Modif selected: " + extra.substr(5));
		updateData(std::stoi(extra.substr(5)));
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}


	TreadmillDriver::RunState state = ctx.m_treadmill.getState();
	if (m_cur_state != state) {
		using  RState = TreadmillDriver::RunState;

		if (state == RState::Stop || (m_not_count && state == RState::Pause)) {
			stopAction();
		} else if (state == RState::Pause) {
//			ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
			m_start->setPassive("rehab_test_protocol_pause.png");
			m_start->setActive("rehab_test_protocol_pause.png");
			m_timer.stopTimer();
		}

		m_cur_state = state;
	}

	ret = Menu::processEvent(std::move(event));
	if (m_event != EventType::None) {
		if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run) {
			if (m_event == EventType::Stop || m_not_count) {
				stopAction();
			} else if (m_event == EventType::Pause) {
//				ctx.m_treadmill.setState(TreadmillDriver::RunState::Pause);
				m_start->setPassive("rehab_test_protocol_pause.png");
				m_start->setActive("rehab_test_protocol_pause.png");
				m_timer.stopTimer();
			}
		}
	}
	return ret;
}

void RehabTestProtocolMenu::turnOffHighLight() {
	for (auto r : m_high_light) {
		r->setVisible(false);
	}
}

void RehabTestProtocolMenu::highLightRow(int row) {
	for (auto r : m_high_light) {
		r->setY(m_rows_start + row * m_row_step);
		r->setVisible(true);
	}
}

void RehabTestProtocolMenu::stopAction() {
	m_running = false;
	ctx.m_session.finish();
	ctx.m_treadmill.setState(TreadmillDriver::RunState::Stop);
	m_timer.stopTimer();
	m_time = 0;
	m_start->setPassive("");
	m_start->setActive("rehab_test_protocol_start.png");
	m_stop->setPassive("rehab_test_protocol_stop.png");
	m_current_interval = 0;
	m_interval_time = 0;
	turnOffHighLight();

	if (m_not_count) {
		m_msg_line_1->setText("");
		m_msg_line_2->setText("");
		m_msg_line_3->setText("");
		m_not_count = false;
	}
	if (m_modif) {
		m_modif->setState(Widget::State::Passive);
	}
}

void RehabTestProtocolMenu::updateData(int id) {
	char buff[100];
	float t;

	m_params = ctx.m_db.getProtocolParams(id);

	m_modif_name->setText(M(m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Modification))));

	std::string duration_units = M(m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationUnit)));
	std::string duration_fmt = "%" + m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationFmt)) + "f";
	std::string speed_units = M(ctx.m_db.getParam("ЕдиницыСкорость"));
	std::string angle_units = M(ctx.m_db.getParam("ЕдиницыУгол"));

	m_duration_unit->setText(duration_units);
	m_speed_unit->setText(speed_units);
	m_angle_unit->setText(angle_units);

	m_intervals_data = ctx.m_db.getIntervals(id);

	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		for (unsigned j = 0; j < TABLE_COLS; ++j) {
			std::string str;

			if (i < m_intervals_data.size() && j < m_intervals_data.at(i).size()) {
				str = m_intervals_data.at(i).at(j);
				switch (j) {
				case 1: // Длительность
					t = std::stof(str);
					if (m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationUnit)) == "мин") {
						t /= 60.;
					}
					sprintf(buff, duration_fmt.c_str(), t);
					str = buff;
					break;
				case 2: // Скорость
					t = std::stof(str);
					str = ctx.m_session.getSpeedString(t);
					break;
				case 3: // Угол
					t = std::stof(str);
					str = ctx.m_session.getAngleString(t);
					break;
				}
			}
			m_table[i][j]->setText(Utils::allTrim(str));
		}
	}
}
