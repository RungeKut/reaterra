#include "RehabUserProtocolsViewMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "ModalCommonDialog.h"
#include "RehabMenu.h"
#include "RehabUserProtocolsEditMenu.h"
#include "WidgetBreadCrumbs.h"
#include "Utils.h"

RehabUserProtocolsViewMenu::RehabUserProtocolsViewMenu()
:	Menu {0, 0, "rehab_user_protocols_view_menu.png", {
	}},
	m_selected_row {m_intervals.size() ? 1 : 0},
	m_page_start {0},
	m_changed {false},
	m_col_cor {1}
{
	LD("Ctor");

	int id = stoi(ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"].at(0));
	m_mode = to_enum<Mode>(stoi(ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"].at(1)));
	m_pro_mode = to_enum<RehabUserProtocolsMenu::ProMode>(stoi(ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"].at(2)));
	m_intervals = ctx.m_db.getIntervals(id);
	m_params = ctx.m_db.getProtocolParams(id);

	std::string &name = m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name));
	std::string &description = m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Description));
	std::string duration_units = M(m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationUnit)));
	std::string speed_units = M(ctx.m_db.getParam("ЕдиницыСкорость"));
	std::string angle_units = M(ctx.m_db.getParam("ЕдиницыУгол"));

	std::string subject_l = "протокол";
	std::string subject_l_uc = "ПРОТОКОЛ";
	std::string subject_la = "протокола";
	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		m_col_cor = 0;
		subject_l = "профиль";
		subject_l_uc = "ПРОФИЛЬ";
		subject_la = "профиля";
		addWidget(std::make_shared<WidgetImageButton>(225, 198, "", "rehab_user_protocols_view_grid_profil.png"))->setState(Widget::State::Disabled);
	}

	std::string title;
	switch (m_mode) {
	case Mode::View:
		title = M("просмотр " + subject_la);
		break;
	case Mode::New:
		title = M("создание " + subject_la);
		break;
	case Mode::Edit:
		title = M("редактирование " + subject_la);
		m_prev_name = name;
		break;
	}

	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this, subject_l_uc](WidgetImageButton::Event e){return backAction(std::make_unique<MainMenu>(), subject_l_uc);}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this, subject_l_uc](WidgetImageButton::Event e){return backAction(std::make_unique<RehabMenu>(), subject_l_uc);}, bc_link_style},
			{M("ПРОФИЛИ"), [this, subject_l_uc](WidgetImageButton::Event e){
					ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Profil))};
					return backAction(std::make_unique<RehabUserProtocolsMenu>(), subject_l_uc);
				}, bc_link_style},
			{title, WIBFunc(), bc_last_style}
		});
	} else {
		addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this, subject_l_uc](WidgetImageButton::Event e){return backAction(std::make_unique<MainMenu>(), subject_l_uc);}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this, subject_l_uc](WidgetImageButton::Event e){return backAction(std::make_unique<RehabMenu>(), subject_l_uc);}, bc_link_style},
			{M("ПРОТОКОЛЫ"), [this, subject_l_uc](WidgetImageButton::Event e){
					ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Protocol))};
					return backAction(std::make_unique<RehabUserProtocolsMenu>(), subject_l_uc);
				}, bc_link_style},
			{title, WIBFunc(), bc_last_style}
		});
	}


	addOnBackAction(std::function<std::unique_ptr<::Event>(WidgetImageButton::Event)> ([this, subject_l_uc](WidgetImageButton::Event e) {
		if (m_mode == Mode::View) {
			return backAction(std::make_unique<RehabUserProtocolsMenu>(), subject_l_uc);
		} else {
			return backAction(std::make_unique<RehabUserProtocolsEditMenu>(), subject_l_uc);
		}
	}));



	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.font_size = 25;
	style.font_style = FontStyle::Bold;

	addWidget(std::make_shared<WidgetTextField>(0, 47, 225, 45, style, M("название")));

	style.text_color_disabled = 0x003062;
	addWidget(std::make_shared<WidgetTextField>(0, 92, 225, 105, style, M("описание\\" + subject_la)));

	m_pgup = addWidget(std::make_shared<WidgetImageButton>(1219, 198, "rehab_user_protocols_view_pgup_active.png", "", "rehab_user_protocols_view_pgup_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start -= TABLE_ROWS;
				updateTable();
				return nullptr;
			}));

	m_pgdn = addWidget(std::make_shared<WidgetImageButton>(1219, 676, "rehab_user_protocols_view_pgdn_active.png", "", "rehab_user_protocols_view_pgdn_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start += TABLE_ROWS;
				updateTable();
				return nullptr;
			}));

	m_thumb = addWidget(std::make_shared<WidgetRectangle>(1227, 0, 44, 10, 0xBEBFBF));

	if (m_mode != Mode::View) {
		m_add_interval = addWidget(std::make_shared<WidgetImageButton>(24, 238,
				"rehab_user_protocols_add_interval_active.png",
				"rehab_user_protocols_add_interval_disabled.png",
				"rehab_user_protocols_add_interval_passive.png",
				[this](WidgetImageButton::Event e) {
					DataSet new_interval = ctx.m_db.getIntervals(-1);
					new_interval.at(0).at(0) = m_intervals.size() + 1;
					m_intervals.push_back(new_interval.at(0));
					if (m_intervals.size() > static_cast<size_t>(m_page_start + TABLE_ROWS)) {
						m_page_start += TABLE_ROWS;
					}
					updateTable();
					m_changed = true;
					return nullptr;
				}));

		style = default_style;
		style.text_color_disabled = 0xFFFFFF;
		style.font_size = 25;
		addWidget(std::make_shared<WidgetTextField>(0, 318, 225, 96, style))->setText(M("ДОБАВИТЬ\\ИНТЕРВАЛ"));
		addWidget(std::make_shared<WidgetTextField>(0, 521, 225, 96, style))->setText(M("УДАЛИТЬ\\ИНТЕРВАЛ"));

		m_delete_interval = addWidget(std::make_shared<WidgetImageButton>(24, 441,
				"rehab_user_protocols_delete_interval_active.png",
				"rehab_user_protocols_delete_interval_disabled.png",
				"rehab_user_protocols_delete_interval_passive.png",
				[this](WidgetImageButton::Event e) {
					m_intervals.pop_back();
					if (m_intervals.size() - 1 < static_cast<size_t>(m_page_start)) {
						m_page_start -= TABLE_ROWS;
						if (m_page_start < 0) {
							m_page_start = 0;
						}
					} else if (m_intervals.size() > static_cast<size_t>(m_page_start + TABLE_ROWS)) {
						m_page_start += TABLE_ROWS;
					}
					updateTable();
					m_changed = true;
					return nullptr;
				}));
	}

	std::array<int, TABLE_COLS + 1> cols_prot = {225, 393, 683, 951, 1219};
	std::array<int, TABLE_COLS + 1> cols_prof = {225, 379, 567, 835, 1028, 1219};
	std::array<int, TABLE_COLS + 1> cols;
	std::copy(cols_prot.begin(), cols_prot.end(), cols.begin());

	std::array<std::tuple<std::string, char, float, float>, TABLE_COLS> format_prot = {{
			{"%d", 'f', 0, 0},
			{ctx.m_session.getSpeedFormat(), 'f', stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMinSpeed() / 10.0)),
					stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMaxForwardSpeed() / 10.0))},
			{"%" + m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationFmt)) + "f", 'f', 1, 999},
			{ctx.m_session.getAngleFormat(), 'f', 0, stof(ctx.m_session.getAngleString(ctx.m_treadmill.getMaxAngle() / 100.0))}
	}};

	std::array<std::tuple<std::string, char, float, float>, TABLE_COLS> format_prof = {{
			{"%d", 'f', 0, 0},
			{"%1.0f", 'f', 1, 7},
			{ctx.m_session.getSpeedFormat(), 'f',stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMinSpeed() / 10.0)),
					stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMaxForwardSpeed() / 10.0))},
			{"%" + m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationFmt)) + "f", 'f', 1, 999},
			{ctx.m_session.getAngleFormat(), 'f', 0, stof(ctx.m_session.getAngleString(ctx.m_treadmill.getMaxAngle() / 100.0))}
	}};

	std::array<std::tuple<std::string, char, float, float>, TABLE_COLS> format;
	std::copy(format_prot.begin(), format_prot.end(), format.begin());

	std::array<int, TABLE_COLS> index_prot = {0,
			static_cast<int>(DbConnector::IntervalData::Speed),
			static_cast<int>(DbConnector::IntervalData::Duration),
			static_cast<int>(DbConnector::IntervalData::Angle)
	};

	std::array<int, TABLE_COLS> index_prof = {0,
			static_cast<int>(DbConnector::IntervalData::Acceleration),
			static_cast<int>(DbConnector::IntervalData::Speed),
			static_cast<int>(DbConnector::IntervalData::Duration),
			static_cast<int>(DbConnector::IntervalData::Angle)
	};
	std::array<int, TABLE_COLS> index;
	std::copy(index_prot.begin(), index_prot.end(), index.begin());
	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		std::copy(cols_prof.begin(), cols_prof.end(), cols.begin());
		std::copy(format_prof.begin(), format_prof.end(), format.begin());
		std::copy(index_prof.begin(), index_prof.end(), index.begin());
	}

	style = rehab_user_protocol_table_style;
	style.text_max_size = 5;
	style.check_func = [this, format, index](std::string s, WidgetTextField &w) {
		m_changed = true;
		try {
			char buff[100];
			if (std::get<1>(format[w.getIdCol()]) == 'f') {
				sprintf(buff, std::get<0>(format[w.getIdCol()]).c_str(), std::stof(s));
				LD("Save value: " + buff + "   " + std::to_string(index[w.getIdCol()]));
				std::string res = buff;
				if (index[w.getIdCol()] == to_base(DbConnector::IntervalData::Speed)) {
					res = std::to_string(ctx.m_session.getSpeedKmH(std::stof(buff)));
				} else if (index[w.getIdCol()] == to_base(DbConnector::IntervalData::Angle)) {
					res = std::to_string(ctx.m_session.getAngleDegree(std::stof(buff)));
				}
				m_intervals.at(m_page_start + w.getIdRow()).at(index[w.getIdCol()]) = res;
				w.setText(buff);
			}
		} catch (std::exception &ex) {
			LE("CATCH in check_func!!! s: [" + s + "]");
		}
		return "";
	};
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {

		m_text_list.push_back({});
		for (unsigned j = 0; j < static_cast<unsigned>(TABLE_COLS - m_col_cor); ++j) {

			if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol) {
				if (j == 2) {
					style.kbd_type = KbdType::Digital;
				} else {
					style.kbd_type = KbdType::Digital | KbdType::DotFlag;
				}
			} else {
				if (j == 1 || j == 3) {
					style.kbd_type = KbdType::Digital;
				} else {
					style.kbd_type = KbdType::Digital | KbdType::DotFlag;
				}
			}
			m_text_list.back().push_back(std::make_shared<WidgetTextField>(cols[j], ROW_START_Y + i * ROW_STEP_Y,
					cols[j + 1] - cols[j], ROW_STEP_Y - 1, style));
			addWidget(m_text_list.back().at(j))->setIdCol(j).setIdRow(i).setLimits(std::get<2>(format[j]), std::get<3>(format[j]));
		}
	}


	style = rehab_user_protocol_table_style;
	style.font_style = FontStyle::Bold;
	style.halign = Align::Left;
	style.lines = 1;
	style.text_max_size = 40;
	style.check_func = [this](std::string s, WidgetTextField &w) {
		m_changed = true;
		m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name)) = s;
		return "";
	};
	(m_name = addWidget(std::make_shared<WidgetTextField>(224, 47, 1056, 46, style)))->setText(name);

	style.font_style = FontStyle::Normal;
	style.text_max_size = 80;
	style.lines = 3;
	style.check_func = [this](std::string s, WidgetTextField &w) {
		m_changed = true;
		m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Description)) = s;
		return "";
	};
	(m_description = addWidget(std::make_shared<WidgetTextField>(224, 92, 1056, 105, style)))->setText(description);

	style.font_style = FontStyle::Bold;
	style.halign = Align::Center;
	style.lines = 1;
	style.text_color_disabled = 0x003264;
	style.check_func = nullptr;
	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		addWidget(std::make_shared<WidgetTextField>(224, 197, 153, 45, style))->setText(M("интервал"));
		addWidget(std::make_shared<WidgetTextField>(379, 197, 189, 45, style))->setText(M("ускорение"));
		(m_speed_title = addWidget(std::make_shared<WidgetTextField>(567, 197, 267, 45, style)))->setText(M("скорость, ") + speed_units);
		(m_duration_title = addWidget(std::make_shared<WidgetTextField>(835, 197, 192, 45, style)))->setText(M("время, ") + duration_units);
		(m_angle_title = addWidget(std::make_shared<WidgetTextField>(1028, 197, 190, 45, style)))->setText(M("угол, ") + angle_units);
	} else {
		addWidget(std::make_shared<WidgetTextField>(224, 197, 169, 45, style))->setText(M("интервал"));
		(m_speed_title = addWidget(std::make_shared<WidgetTextField>(393, 197, 290, 45, style)))->setText(M("скорость, ") + speed_units);
		(m_duration_title = addWidget(std::make_shared<WidgetTextField>(683, 197, 268, 45, style)))->setText(M("время, ") + duration_units);
		(m_angle_title = addWidget(std::make_shared<WidgetTextField>(951, 197, 267, 45, style)))->setText(M("угол, ") + angle_units);
	}

	updateTable();
}

RehabUserProtocolsViewMenu::~RehabUserProtocolsViewMenu() {
	LD("Dtor");
}

Menu* RehabUserProtocolsViewMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	LD("Event enter" + event->getName());

	if (m_modal && m_modal->getType() == "SAVE" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();

		if (m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name)).empty()) {
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
					M("ВВЕДИТЕ НАЗВАНИЕ"), "SVER", Flags::YellowTitle | Flags::CloseButton,
					"icon_save_cross_yellow.png"));
		} else {

			bool duplicated = false;

			if (m_mode == Mode::New || (m_mode == Mode::Edit && m_prev_name != m_params.at(0).at(to_base(DbConnector::ProtocolParam::Name)))) {
				DataSet protocols = ctx.m_db.getProtocols(m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil ? "USER_PROF" : "USER");
				for (const auto &p : protocols) {
					if (p.at(static_cast<int>(DbConnector::ProtocolList::Name)) == m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name))) {
						duplicated = true;
						break;
					}
				}
			}

			std::string subject_l = "ПРОТОКОЛ";
			if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
				subject_l = "ПРОФИЛЬ";
			}

			if (duplicated) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
						M(subject_l + " С ТАКИМ НАЗВАНИЕМ\\УЖЕ СУЩЕСТВУЕТ"), "SVDB", Flags::YellowTitle | Flags::CloseButton,
						"icon_protocol_exclamation_yellow.png"));
			} else {

				//TODO: Заменить new на смарт указатели
				if (m_mode == Mode::Edit) {
					ctx.m_db.updateProtocol(std::stoi(ctx.m_prev_menu["UserProfiles"].at(1)), m_params, m_intervals);
				} else {
					ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(ctx.m_db.insertProtocol(m_params, m_intervals,
							m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol ? "USER" : "USER_PROF"))};
				}
				ret = m_next_menu.release();
			}
		}
	} else if (m_modal && m_modal->getType() == "SAVE" && event->getType() == Event::EventType::ModalNo) {
		m_modal.reset();
		ret = m_next_menu.release();
	} else if (event->getType() == Event::EventType::ModalCancel || event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	LD("Event processed");
	return ret;
}

void RehabUserProtocolsViewMenu::updateTable() {
	unsigned active = m_intervals.size() - m_page_start;
	char buff[100];

	if (m_mode != Mode::View) {
		m_name->setState(Widget::State::Passive);
		m_description->setState(Widget::State::Passive);

		if (m_intervals.size() >= (m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol ? 15 : 25)) {
			m_add_interval->setState(Widget::State::Disabled);
		} else if (m_intervals.size() <= 1) {
			m_delete_interval->setState(Widget::State::Disabled);
		} else {
			m_add_interval->setState(Widget::State::Passive);
			m_delete_interval->setState(Widget::State::Passive);
		}

	}

	std::string duration_fmt = "%" + m_params.at(0).at(static_cast<int>(DbConnector::ProtocolParam::DurationFmt)) + "f";
	std::string speed_fmt = ctx.m_session.getSpeedFormat();
	std::string angle_fmt = ctx.m_session.getAngleFormat();

	LD("Before update table");
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		if (i < active) {
			if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol) {
				m_text_list.at(i).at(0)->setText(std::to_string(m_page_start + i + 1));
				m_text_list.at(i).at(1)->setText(ctx.m_session.getSpeedString(std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Speed)))));
				sprintf(buff, duration_fmt.c_str(), std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Duration))));
				m_text_list.at(i).at(2)->setText(Utils::allTrim(buff));
				m_text_list.at(i).at(3)->setText(ctx.m_session.getAngleString(std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Angle)))));
				for (int j = 1; j < 4; ++j) {
					if (m_mode != Mode::View) {
						m_text_list.at(i).at(j)->setState(Widget::State::Passive);
					} else {
						m_text_list.at(i).at(j)->setState(Widget::State::Disabled);
					}
				}
			} else {
				m_text_list.at(i).at(0)->setText(std::to_string(m_page_start + i + 1));
				m_text_list.at(i).at(1)->setText(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Acceleration)));
				m_text_list.at(i).at(2)->setText(ctx.m_session.getSpeedString(std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Speed)))));
				sprintf(buff, duration_fmt.c_str(), std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Duration))));
				m_text_list.at(i).at(3)->setText(Utils::allTrim(buff));
				m_text_list.at(i).at(4)->setText(ctx.m_session.getAngleString(std::stof(m_intervals.at(m_page_start + i).at(static_cast<int>(DbConnector::IntervalData::Angle)))));
				for (int j = 1; j < 5; ++j) {
					if (m_mode != Mode::View) {
						m_text_list.at(i).at(j)->setState(Widget::State::Passive);
					} else {
						m_text_list.at(i).at(j)->setState(Widget::State::Disabled);
					}
				}
			}
		} else {
			for (auto &s : m_text_list.at(i)) {
				s->setText("").setState(Widget::State::Disabled);
			}
		}
	}
	LD("After update table");

	if (m_intervals.size() - m_page_start > TABLE_ROWS) {
		m_pgdn->setState(Widget::State::Passive);
	} else {
		m_pgdn->setState(Widget::State::Disabled);
	}
	if (m_page_start) {
		m_pgup->setState(Widget::State::Passive);
	} else {
		m_pgup->setState(Widget::State::Disabled);
	}
	if (m_intervals.size() > TABLE_ROWS) {
		m_thumb->setY(255 + 434. * m_page_start / m_intervals.size());
		m_thumb->setHeight(434. * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_intervals.size());
		if (m_thumb->getHeight() < 20) {
			m_thumb->setY(255 + 434. - 20);
			m_thumb->setHeight(20);
		}
		m_thumb->setVisible(true);
	} else {
		m_thumb->setVisible(false);
	}
}

std::unique_ptr<MenuEvent> RehabUserProtocolsViewMenu::backAction(std::unique_ptr<Menu> menu, std::string subject) {
	std::unique_ptr<MenuEvent> ret = nullptr;
	if (m_changed) {
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::make_unique<ModalCommonDialog>(M("СОХРАНЕНИЕ ИЗМЕНЕНИЙ"), M("СОХРАНИТЬ ") + M(subject) + "?", "SAVE",
				Flags::GreenTitle | Flags::YesNoCancel, "icon_save_green.png");
		m_next_menu = std::move(menu);
	} else {
		ret = std::make_unique<MenuEvent>(std::move(menu));
	}
	return ret;
}
