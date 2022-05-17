#include "PatientHistoryMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalPatientHistoryFilter.h"
#include "PatientListMenu.h"
#include "WidgetBreadCrumbs.h"

PatientHistoryMenu::PatientHistoryMenu()
:	Menu {0, 0, "patient_history_menu.png"},
m_history {ctx.m_db.getHistory()},
m_page_start_v {0},
m_page_start_h {0}
{
	LD("Ctor");

	m_page_h_size = ctx.m_treadmill.isSasExists() ? 3 : 2;

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M("история пациентов"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	m_pgup_v = addWidget(std::make_shared<WidgetImageButton>(1225, 54, "rehab_user_protocols_pgup_active.png", "", "rehab_user_protocols_pgup_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start_v -= TABLE_ROWS;
				updateTable();
				return nullptr;
			}));

	m_pgdn_v = addWidget(std::make_shared<WidgetImageButton>(1225, 539 + 91, "rehab_user_protocols_pgdn_active.png", "", "rehab_user_protocols_pgdn_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start_v += TABLE_ROWS;
				updateTable();
				return nullptr;
			}));

	m_thumb_v = addWidget(std::make_shared<WidgetRectangle>(1226, 0, 45, 10, 0xBEBFBF));

	m_pgup_h = addWidget(std::make_shared<WidgetImageButton>(0, 683, "patient_history_page_left_active.png", "", "patient_history_page_left_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start_h--;
				updateTable();
				return nullptr;
			}));

	m_pgdn_h = addWidget(std::make_shared<WidgetImageButton>(1153, 683, "patient_history_page_right_active.png", "", "patient_history_page_right_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start_h++;
				updateTable();
				return nullptr;
			}));

	m_thumb_h = addWidget(std::make_shared<WidgetRectangle>(55, 692, 1106, 45, 0xA8A9A9));

	m_filter = addWidget(std::make_shared<WidgetImageButton>(1217, 683, "patient_history_filter_active.png", "", "patient_history_filter_passive.png",
			[this](WidgetImageButton::Event e) {
				m_active_widget = nullptr;
				ctx.m_prev_menu["PatientHistoryFilters"] = {
								"PatientHistoryFilters",
								m_from_filter,
								m_to_filter,
								m_family_filter
						};
				m_modal = std::make_unique<ModalPatientHistoryFilter>();
				return nullptr;
			}));

	cols = {
			{0, 200, 400, 600, 780, 931, 1050, 1216},
			{0, 200, 400, 550, 680, 800, 950, 1216},
			{0, 200, 400, 600, 790, 1000, 1216},
		};

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.halign = Align::Center;
	style.rect_color_active = to_base(Colors::None);;
	style.font_size = 17;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = to_base(Colors::Black);
	style.lines = 1;
	for (size_t i = 0; i < TABLE_ROWS; ++i) {
		m_text_list.push_back({});
		for (size_t k = 0; k < m_page_h_size; ++k) {
			m_text_list.back().push_back({});
			for (size_t j = 0; j < cols.at(k).size() - 1; ++j) {

				m_text_list.back().back().push_back(std::make_shared<WidgetTextField>(cols.at(k).at(j), ROW_START_Y + i * ROW_STEP_Y,
						cols.at(k).at(j + 1) - cols.at(k).at(j), ROW_STEP_Y - 1, style));
				addWidget(m_text_list.back().back().at(j))->setIdCol(j);
			}
		}
	}

	style.font_size = 19;
	style.font_style = FontStyle::Bold;
	style.halign = Align::Center;
	style.lines = 2;
	style.text_color_disabled = 0x003264;

	for (size_t k = 0; k < m_page_h_size; ++k) {
		m_title_list.push_back({});
		m_bar_list.push_back({});
		for (size_t j = 0; j < cols.at(k).size() - 1; ++j) {
			int x = cols.at(k).at(j);
			int w = cols.at(k).at(j + 1) - cols.at(k).at(j);
			WidgetTextFieldSP widget = addWidget(std::make_shared<WidgetTextField>(x, 46, w, 44, style));
			m_title_list.back().push_back(widget);
			if (j == 0) {
				widget->setText(M("Время"));
			} else if (j == 1) {
				widget->setText(M("ФИО"));
			} else {
				switch (k) {
				case 0:
					switch(j) {
					case 2:
						widget->setText(M("Профиль"));
						break;
					case 3:
						widget->setText(M("Длительность") + "\\" + M("ч:мин:с"));
						break;
					case 4:
						widget->setText(M("Расстояние") + "\\" + M(ctx.m_db.getParam("ЕдиницыРасстояние")));
						break;
					case 5:
						widget->setText(M("Скорость") + "\\" + M(ctx.m_db.getParam("ЕдиницыСкорость")));
						break;
					case 6:
						widget->setText(M("Ускорение") + "\\" + M("м/с2"));
						break;
					}
					break;

				case 1:
					switch(j) {
					case 2:
						widget->setText(M("Угол") + "\\" + M(ctx.m_db.getParam("ЕдиницыУгол")));
						break;
					case 3:
						widget->setText(M("Пульс") + "\\" + M("уд/мин"));
						break;
					case 4:
						widget->setText(M("Энергия") + "\\" + M(ctx.m_db.getParam("ЕдиницыЭнергия")));
						break;
					case 5:
						widget->setText(M("Мощность") + "\\" + M(ctx.m_db.getParam("ЕдиницыМощность")));
						break;
					case 6:
						widget->setText(M("Мощность аэробная") + "\\" + M(ctx.m_db.getParam("ЕдиницыМощностьАэробная")));
						break;
					}
					break;

				case 2:
					switch(j) {
					case 2:
						widget->setText(M("Режим АРС"));
						break;
					case 3:
						widget->setText(M("Длина шага") + "\\" + M("см"));
						break;
					case 4:
						widget->setText(M("Симметрия\\левая нога, %"));
						break;
					case 5:
						widget->setText(M("Симметрия\\правая нога, %"));
						break;
					}
					break;
				}
			}
			widget->setVisible(false);
			m_bar_list.back().push_back(std::make_shared<WidgetImageButton>(x + w, 46, "", "patient_history_vline.png"));
			addWidget(m_bar_list.back().back())->setState(Widget::State::Disabled)->setVisible(false);
		}
	}

	updateTable();
}

PatientHistoryMenu::~PatientHistoryMenu() {
	LD("Dtor");
}

Menu* PatientHistoryMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	if (m_modal && m_modal->getType() == "PFLT"
			&& (event->getType() == Event::EventType::ModalYes || event->getType() == Event::EventType::ModalCancel)) {
		m_from_filter = ctx.m_prev_menu["PatientHistoryFilters"].at(1);
		m_to_filter = ctx.m_prev_menu["PatientHistoryFilters"].at(2);
		m_family_filter = ctx.m_prev_menu["PatientHistoryFilters"].at(3);
		m_history = ctx.m_db.getHistory(m_from_filter, m_to_filter, m_family_filter);

		m_modal.reset();
		m_page_start_v = 0;
		updateTable();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}

void PatientHistoryMenu::updateTable() {
	unsigned active = m_history.size() - m_page_start_v;

	for (size_t i = 0; i < m_title_list.size(); ++i) {
		bool vis = false;
		if (i == m_page_start_h) {
			vis = true;
		}
		for (auto &w : m_title_list.at(i)) {
			w->setVisible(vis);
		}
	}

	LD("Before update table");
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		for (auto &v : m_text_list.at(i)) {
			for (auto &s : v) {
				s->setVisible(false);
			}
		}

		WidgetTextFieldSPVec w = m_text_list.at(i).at(m_page_start_h);
		if (i < active) {


			for (size_t j = 0; j < cols.at(m_page_start_h).size() - 1; ++j) {
				if (j == 0) {
					w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Time)));
				} else if (j == 1) {
					w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Fio)));
				} else {
					switch (m_page_start_h) {
					case 0:
						switch(j) {
						case 2:
							w.at(j)->setText(M(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Profile))));
							break;
						case 3: {
							unsigned dur = std::stoi(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Duration)));
							char dur_str[10];
							sprintf(dur_str, "%02d:%02d:%02d", dur / 60 / 60, dur / 60 % 60, dur % 60);
							w.at(j)->setText(dur_str);
							break;}
						case 4: {
							float dist = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Distance)));
							w.at(j)->setText(ctx.m_session.getDistanceString(dist));
							break; }
						case 5: {
							float speed = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Speed)));
							w.at(j)->setText(ctx.m_session.getSpeedString(speed));
							break; }
						case 6:
							float accel = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Acceleration)));
							w.at(j)->setText(ctx.m_session.getAccelString(accel));
						}
						break;

					case 1:
						switch(j) {
						case 2: {
							float angle = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Angle)));
							w.at(j)->setText(ctx.m_session.getAngleString(angle));
							break; }
						case 3:
							w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::HRate)));
							break;
						case 4: {
							float energy = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::Energy)));
							w.at(j)->setText(ctx.m_session.getEnergyString(energy));
							break; }
						case 5: {
							float power = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::PowerMech)));
							w.at(j)->setText(ctx.m_session.getPowerMechString(power));
							break; }
						case 6: {
							float power_aero = std::stof(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::PowerAero)));
							w.at(j)->setText(ctx.m_session.getPowerAeroString(power_aero));
							break; }
						}
						break;
					case 2:
						switch(j) {
						case 2: {
							w.at(j)->setText(M(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::SasMode))));
							break; }
						case 3: {
							w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::SasStepLength)));
							break; }
						case 4:
							w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::SasSymmetryLeft)));
							break;
						case 5: {
							w.at(j)->setText(m_history.at(m_page_start_v + i).at(to_base(DbConnector::History::SasSymmetryRight)));
							break; }
						}
						break;
					}
				}
				w.at(j)->setVisible(true);
			}
		}
	}
	LD("After update table");

	if (m_history.size() - m_page_start_v > TABLE_ROWS) {
		m_pgdn_v->setState(Widget::State::Passive);
	} else {
		m_pgdn_v->setState(Widget::State::Disabled);
	}
	if (m_page_start_v) {
		m_pgup_v->setState(Widget::State::Passive);
	} else {
		m_pgup_v->setState(Widget::State::Disabled);
	}
	if (m_history.size() > TABLE_ROWS) {
		m_thumb_v->setY(101 + (436. + 91) * m_page_start_v / m_history.size());
		m_thumb_v->setHeight((436. + 91) * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_history.size());
		if (m_thumb_v->getHeight() < 20) {
			m_thumb_v->setY(101 + (436. + 91) - 20);
			m_thumb_v->setHeight(20);
		}
		m_thumb_v->setVisible(true);
	} else {
		m_thumb_v->setVisible(false);
	}

	if (m_page_start_h < m_page_h_size - 1) {
		m_pgdn_h->setState(Widget::State::Passive);
	} else {
		m_pgdn_h->setState(Widget::State::Disabled);
	}
	if (m_page_start_h) {
		m_pgup_h->setState(Widget::State::Passive);
	} else {
		m_pgup_h->setState(Widget::State::Disabled);
	}

	if (m_page_h_size > 1) {
		m_thumb_h->setX(55 + 1106. * m_page_start_h / m_page_h_size);
		m_thumb_h->setWidth(1106. / m_page_h_size);
		if (m_thumb_h->getWidth() < 20) {
			m_thumb_h->setX(55 + 1106. - 20);
			m_thumb_h->setWidth(20);
		}
		m_thumb_h->setVisible(true);
	} else {
		m_thumb_h->setVisible(false);
	}

	for (size_t i = 0; i < m_bar_list.size(); ++i) {
		bool visible = false;
		if (i == m_page_start_h) {
			visible = true;
		}
		for (const auto &w : m_bar_list.at(i)) {
			w->setVisible(visible);
		}
	}

}
