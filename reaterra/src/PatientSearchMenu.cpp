#include "PatientSearchMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "PatientListMenu.h"
#include "WidgetBreadCrumbs.h"

//TODO: Объединить функциональность PatientListMenu и PatientSearchMenu
PatientSearchMenu::PatientSearchMenu()
:	Menu {0, 0, "patient_search_menu.png"},
m_order_field {"id"},
m_order_dir {true},
m_patients {ctx.m_db.getPatients(m_order_field, m_order_dir)},
m_selected_row {m_patients.size() ? 1 : 0},
m_page_start {0}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M("поиск пациентов"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	if (ctx.m_prev_menu.find("PatientList") != ctx.m_prev_menu.end()) {
		if (m_patients.size()) {
			m_selected_row = 1;
		}

		for (size_t i = 0; i < m_patients.size(); ++i) {
			if (m_patients.at(i).at(to_base(DbConnector::PatientList::Id)) == ctx.m_prev_menu["PatientList"].at(1)) {
				m_selected_row = i + 1;
				break;
			}
		}

		while (m_selected_row > TABLE_ROWS) {
			m_selected_row -= TABLE_ROWS;
			m_page_start += TABLE_ROWS;
		}
	}

	m_set = addWidget(std::make_shared<WidgetImageButton>(1028, 666, "patient_list_menu_set_active.png", "patient_list_menu_set_disabled.png", "",
			[this](WidgetImageButton::Event e) {
//				ctx.m_patient = getSelectedPatientFio();
//				ctx.m_patient_id = getSelectedPatientId();
				setPatient(getSelectedPatientFio(),	getSelectedPatientId(), getSelectedPatientWeight(), getSelectedPatientProfile());
				m_unset->setState(Widget::State::Passive);
				return nullptr;
			}));
	m_unset = addWidget(std::make_shared<WidgetImageButton>(1149, 666, "patient_list_menu_unset_active.png", "patient_list_menu_unset_disabled.png", "",
			[this](WidgetImageButton::Event e) {
//				ctx.m_patient = "";
//				ctx.m_patient_id = -1;
				setPatient("", -1, 0, 0);
				m_unset->setState(Widget::State::Disabled);
				return nullptr;
			}));
	if (ctx.m_patient.empty()) {
		m_unset->setState(Widget::State::Disabled);
	}

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.halign = Align::Center;
	style.lines = 6;
	style.font_size = 26;
	style.rect_color_active = to_base(Colors::None);;
	style.text_color_disabled = 0x003264;
	addWidget(std::make_shared<WidgetTextField>(90, 47, 555, 63, style))
		->setText(M("Поиск по первым буквам фамилии"))
		.setState(Widget::State::Disabled);

	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(0, 653, 1006, 93, style))
		->setText(M("активация учетной записи"))
		.setState(Widget::State::Disabled);

	style.kbd_type = KbdType::Alpha;
	style.rect_color_active = 0x87CEEA;
	style.text_color_passive = to_base(Colors::Black);
	style.text_color_active = to_base(Colors::Black);
	style.halign = Align::Left;
	style.text_max_size = 10;
	style.check_func = [this](std::string s, WidgetTextField &w) {
		m_filter = s;
		m_patients = ctx.m_db.getPatients(m_order_field, m_order_dir, m_filter);
		m_selected_row = m_patients.size() ? 1 : 0;
		m_page_start = 0;
		updateTable();
		return "";
	};
	addWidget(std::make_shared<WidgetTextField>(651, 56, 623, 44, style))
		->setState(Widget::State::Passive);
	style.check_func = {};

	m_pgup = addWidget(std::make_shared<WidgetImageButton>(1225, 116, "rehab_user_protocols_pgup_active.png", "", "rehab_user_protocols_pgup_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start -= TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_pgdn = addWidget(std::make_shared<WidgetImageButton>(1225, 601, "rehab_user_protocols_pgdn_active.png", "", "rehab_user_protocols_pgdn_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start += TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_thumb = addWidget(std::make_shared<WidgetRectangle>(1226, 0, 45, 10, 0xBEBFBF));

	std::array<int, TABLE_COLS + 1> cols = {0, 75, 325, 514, 791, 931, 1218};

	style.kbd_type = KbdType::None;
	style.text_color_passive = to_base(Colors::BlueOnYellow);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = to_base(Colors::Black);
	style.rect_color_active = 0xB3B3B3;
	style.lines = 1;
	style.halign = Align::Center;
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		m_list.push_back(std::make_shared<WidgetImageButton>(0, ROW_START_Y + i * ROW_STEP_Y, "patient_list_menu_sel_" + std::to_string(i + 1) + ".png", "", "",
				[this, i](WidgetImageButton::Event e) {
					for (unsigned k = 0; k < m_list.size(); ++k) {
						if (k != i) {
							m_list.at(k)->clearTrigger();
						}
					}
					m_selected_row = i + 1;
					if (ctx.m_prev_menu.find("PatientList") != ctx.m_prev_menu.end()) {
						ctx.m_prev_menu["PatientList"].at(1) = std::to_string(getSelectedPatientId());
					}
					LD("Selected: " + std::to_string(m_selected_row));
					return nullptr;
			}, WidgetImageButton::Type::Trigger));
		addWidget(m_list.back());

		m_text_list.push_back({});
		for (unsigned j = 0; j < TABLE_COLS; ++j) {

			m_text_list.back().push_back(std::make_shared<WidgetTextField>(cols[j], ROW_START_Y + i * ROW_STEP_Y,
					cols[j + 1] - cols[j], ROW_STEP_Y - 1, style));
			addWidget(m_text_list.back().at(j))->setIdCol(j).setIdRow(i);
		}
	}

	style.font_size = 24;
	style.font_style = FontStyle::Bold;
	style.halign = Align::Center;
	style.lines = 1;
	style.text_color_disabled = 0x003264;
	style.check_func = [this](std::string s, WidgetTextField &w) {
		if (m_order_field == w.getTextId()) {
			m_order_dir = !m_order_dir;
		} else {
			m_order_field = w.getTextId();
			m_order_dir = true;
		}
		m_patients = ctx.m_db.getPatients(m_order_field, m_order_dir, m_filter);
		if (m_selected_row) {
			m_list.at(m_selected_row - 1)->clearTrigger();
		}
		m_selected_row = m_patients.size() ? 1 : 0;
		m_page_start = 0;
		updateTable();
		return "";
	};
	style.update_func = [this](WidgetTextField &w) {
		std::string title;
		if (w.getTextId() == "id") {
			title = "№";
		} else if (w.getTextId() == "Фамилия") {
			title = "фамилия";
		} else if (w.getTextId() == "Имя") {
			title = "имя";
		} else if (w.getTextId() == "Отчество") {
			title = "отчество";
		} else if (w.getTextId() == "Возраст") {
			title = "возраст";
		} else if (w.getTextId() == "Врач") {
			title = "врач";
		}

		if (m_order_field == w.getTextId()) {
			return M(title) + (m_order_dir ? "\xE2\x86\x91" : "\xE2\x86\x93");
		} else {
			return M(title);
		}
	};
	addWidget(std::make_shared<WidgetTextField>(0, 109, 75, 50, style))->setText(".", "id").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(75, 109, 250, 50, style))->setText(".", "Фамилия").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(325, 109, 188, 50, style))->setText(".", "Имя").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(514, 109, 277, 50, style))->setText(".", "Отчество").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(791, 109, 140, 50, style))->setText(".", "Возраст").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(931, 109, 286, 50, style))->setText(".", "Врач").setState(Widget::State::Passive);

	updateTable();
}

PatientSearchMenu::~PatientSearchMenu() {
	LD("id: " + ctx.m_prev_menu["PatientList"].at(1));
	LD("Dtor");
}


void PatientSearchMenu::updateTable() {
	LD(DbConnector::dumpDataSet(m_patients, "m_patients"));
	unsigned active = m_patients.size() - m_page_start;

	LD("Before update table");
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		if (i < active) {
			m_text_list.at(i).at(0)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::Number)));
			m_text_list.at(i).at(1)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::LastName)));
			m_text_list.at(i).at(2)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::FirstName)));
			m_text_list.at(i).at(3)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::MiddleName)));
			m_text_list.at(i).at(4)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::Age)));
			m_text_list.at(i).at(5)->setText(m_patients.at(m_page_start + i).at(to_base(DbConnector::PatientList::Doctor)));
			for (int j = 0; j < TABLE_COLS; ++j) {
					m_text_list.at(i).at(j)->setState(Widget::State::Disabled);
			}
			m_list.at(i)->setState(Widget::State::Passive);
		} else {
			m_list.at(i)->setState(Widget::State::Disabled);
			for (auto &s : m_text_list.at(i)) {
				s->setText("").setState(Widget::State::Disabled);
			}
		}
		m_list.at(i)->clearTrigger();
	}
	if (m_selected_row) {
		m_list.at(m_selected_row - 1)->setTrigger();
		if (ctx.m_prev_menu.find("PatientList") != ctx.m_prev_menu.end()) {
			ctx.m_prev_menu["PatientList"].at(1) = std::to_string(getSelectedPatientId());
		}
	}
	LD("After update table");

	if (m_patients.size() - m_page_start > TABLE_ROWS) {
		m_pgdn->setState(Widget::State::Passive);
	} else {
		m_pgdn->setState(Widget::State::Disabled);
	}
	if (m_page_start) {
		m_pgup->setState(Widget::State::Passive);
	} else {
		m_pgup->setState(Widget::State::Disabled);
	}
	if (m_patients.size() > TABLE_ROWS) {
		m_thumb->setY(163 + 436. * m_page_start / m_patients.size());
		m_thumb->setHeight(436. * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_patients.size());
		if (m_thumb->getHeight() < 20) {
			m_thumb->setY(163 + 436. - 20);
			m_thumb->setHeight(20);
		}
		m_thumb->setVisible(true);
	} else {
		m_thumb->setVisible(false);
	}

	if (m_patients.size()) {
		m_set->setState(Widget::State::Passive);
	} else {
		m_set->setState(Widget::State::Disabled);
	}
}

int PatientSearchMenu::getSelectedPatientId() {
	return std::stoi(m_patients.at(m_page_start + m_selected_row - 1).at(to_base(DbConnector::PatientList::Id)));
}

std::string PatientSearchMenu::getSelectedPatientFio() {
	std::vector<std::string> rec = m_patients.at(m_page_start + m_selected_row - 1);

	return Utils::getFio(rec.at(to_base(DbConnector::PatientList::LastName)),
			rec.at(to_base(DbConnector::PatientList::FirstName)),
			rec.at(to_base(DbConnector::PatientList::MiddleName)));
}

float PatientSearchMenu::getSelectedPatientWeight() {
	std::vector<std::string> rec = m_patients.at(m_page_start + m_selected_row - 1);
	return stof(rec.at(to_base(DbConnector::PatientList::Weight)));
}

int PatientSearchMenu::getSelectedPatientProfile() {
	std::vector<std::string> rec = m_patients.at(m_page_start + m_selected_row - 1);
	return stoi(rec.at(to_base(DbConnector::PatientList::ProfileId)));
}
