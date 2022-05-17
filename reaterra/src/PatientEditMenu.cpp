#include "PatientEditMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalAuthDoctor.h"
#include "ModalCommonDialog.h"
#include "PatientListMenu.h"
#include "PatientNewMenu.h"
#include "WidgetBreadCrumbs.h"

PatientEditMenu::PatientEditMenu()
:	Menu {0, 0, "patient_edit_menu.png"},
m_order_field {"id"},
m_order_dir {true},
m_selected_row {0},
m_page_start {0},
m_first_update {true}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M("редактор базы пациентов"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Left;
	style.lines = 6;
	style.font_size = 24;
	style.rect_color_active = to_base(Colors::None);

	addWidget(std::make_shared<WidgetImageButton>(0, 646, "patient_edit_menu_add_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				if (m_patients.size()) {
					int id = getSelectedPatientId();
					ctx.m_prev_menu["PatientList"] = {"PatientList", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
				} else {
					ctx.m_prev_menu.erase("PatientList");
				}
				ctx.m_prev_menu["PatientNewMenu.params"] = {"PatientNewMenu", std::to_string(-1), std::to_string(to_base(PatientNewMenu::Mode::New))};
				return std::make_unique<MenuEvent>(std::make_unique<PatientNewMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(20, 646, 405, 100, style, M("добавить пациента"))));
	m_edit = addWidget(std::make_shared<WidgetImageButton>(427, 646, "patient_edit_menu_modify_active.png", "patient_edit_menu_modify_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				if (m_patients.size()) {
					int id = getSelectedPatientId();
					ctx.m_prev_menu["PatientList"] = {"PatientList", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
				} else {
					ctx.m_prev_menu.erase("PatientList");
				}
				ctx.m_prev_menu["PatientNewMenu.params"] = {"PatientNewMenu", std::to_string(getSelectedPatientId()), std::to_string(to_base(PatientNewMenu::Mode::Edit))};
				return std::make_unique<MenuEvent>(std::make_unique<PatientNewMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(447, 646, 406, 100, style, M("редактировать\\данные пациента"))));
	m_delete = addWidget(std::make_shared<WidgetImageButton>(855, 646, "patient_edit_menu_delete_active.png", "patient_edit_menu_delete_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				std::vector<std::string> rec = m_patients.at(m_page_start + m_selected_row - 1);
				std::string first = rec.at(to_base(DbConnector::PatientList::FirstName));
				std::string middle = rec.at(to_base(DbConnector::PatientList::MiddleName));
				std::string last = rec.at(to_base(DbConnector::PatientList::LastName));
				using Flags = ModalCommonDialog::Flags;

				if (ctx.m_patient_id == std::stoi(rec.at(to_base(DbConnector::PatientList::Id)))) {
					m_modal = std::make_unique<ModalCommonDialog>(M("УДАЛЕНИЕ НЕВОЗМОЖНО"),
							M("НЕЛЬЗЯ УДАЛИТЬ\\ТЕКУЩЕГО ПАЦИЕНТА"), "PDE1", Flags::YellowTitle | Flags::CloseButton,
							"icon_treadmill_exclamation_yellow.png");
				} else  {
					m_modal = std::make_unique<ModalCommonDialog>(M("ВНИМАНИЕ! УДАЛИТЬ\\ДАННЫЕ ПАЦИЕНТА?"),
							last + "\\" + first + " " + middle,
							"PDEL", Flags::YellowTitle | Flags::YesCancel, "icon_protocol_cross_yellow.png");
				}
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(875, 646, 405, 100, style, M("удалить\\данные пациента"))));

	m_pgup = addWidget(std::make_shared<WidgetImageButton>(1225, 54, "rehab_user_protocols_pgup_active.png", "", "rehab_user_protocols_pgup_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start -= TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_pgdn = addWidget(std::make_shared<WidgetImageButton>(1225, 594, "rehab_user_protocols_pgdn_active.png", "", "rehab_user_protocols_pgdn_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start += TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_thumb = addWidget(std::make_shared<WidgetRectangle>(1227, 0, 44, 10, 0xBEBFBF));

	std::array<int, TABLE_COLS + 1> cols = {0, 75, 325, 514, 791, 931, 1218};

	style.kbd_type = KbdType::None;
	style.text_color_passive = to_base(Colors::BlueOnYellow);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = to_base(Colors::Black);
	style.rect_color_active = 0xB3B3B3;
	style.lines = 1;
	style.halign = Align::Center;
	style.font_size = 26;
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		m_list.push_back(std::make_shared<WidgetImageButton>(0, ROW_START_Y + i * ROW_STEP_Y, "patient_list_menu_sel_" + std::to_string(i + 1) + ".png", "", "",
				[this, i](WidgetImageButton::Event e) {
					for (unsigned k = 0; k < m_list.size(); ++k) {
						if (k != i) {
							m_list.at(k)->clearTrigger();
						}
					}
					m_selected_row = i + 1;
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
		m_patients = ctx.m_db.getPatients(m_order_field, m_order_dir);
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
	addWidget(std::make_shared<WidgetTextField>(0, 46, 75, 50, style))->setText(".", "id").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(75, 46, 250, 50, style))->setText(".", "Фамилия").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(325, 46, 188, 50, style))->setText(".", "Имя").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(514, 46, 277, 50, style))->setText(".", "Отчество").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(791, 46, 140, 50, style))->setText(".", "Возраст").setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(931, 46, 286, 50, style))->setText(".", "Врач").setState(Widget::State::Passive);
}

PatientEditMenu::~PatientEditMenu() {
	LD("Dtor");
}

Menu* PatientEditMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	if (m_first_update) {
		m_first_update = false;
		initData();
		updateTable();
	}

	if (m_modal && m_modal->getType() == "PDEL" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();

		int id = getSelectedPatientId();
		ctx.m_db.deletePatient(id);
		int index = m_page_start + m_selected_row - 1;

		if (index == 0 && m_patients.size() == 1) {
			m_selected_row = 0;
			ctx.m_prev_menu.erase("PatientList");
			m_patients.clear();
			Widget::State st = m_patients.size() ? Widget::State::Passive : Widget::State::Disabled;
			m_delete->setState(st);
			m_edit->setState(st);
		} else {
			if (index == 0) {
				index++;
			} else {
				index--;
			}
			std::string new_id = m_patients.at(index).at(static_cast<int>(DbConnector::PatientList::Id));
			m_patients = ctx.m_db.getPatients(m_order_field, m_order_dir);
			recalculatePage(new_id);
			ctx.m_prev_menu["PatientList"] = {"PatientList", new_id, std::to_string(m_page_start), std::to_string(m_selected_row)};
		}
		updateTable();
	} else if (m_modal && m_modal->getType() == "PDEL" && event->getType() == Event::EventType::ModalCancel) {
		m_modal.reset();
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}

void PatientEditMenu::updateTable() {
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
		m_thumb->setY(101 + 490. * m_page_start / m_patients.size());
		m_thumb->setHeight(490. * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_patients.size());
		if (m_thumb->getHeight() < 20) {
			m_thumb->setY(101 + 490. - 20);
			m_thumb->setHeight(20);
		}
		m_thumb->setVisible(true);
	} else {
		m_thumb->setVisible(false);
	}
}

int PatientEditMenu::getSelectedPatientId() {
	return std::stoi(m_patients.at(m_page_start + m_selected_row - 1).at(to_base(DbConnector::PatientList::Id)));
}

void PatientEditMenu::recalculatePage(std::string id) {
	m_page_start = 0;
	if (!m_patients.size()) {
		m_selected_row = 0;
		return;
	}

	for (unsigned i = 0; i < m_patients.size(); ++i) {
		if (m_patients.at(i).at(static_cast<int>(DbConnector::PatientList::Id)) == id) {
			m_selected_row = i + 1;
			break;
		}
	}

	while (m_selected_row > TABLE_ROWS) {
		m_selected_row -= TABLE_ROWS;
		m_page_start += TABLE_ROWS;
	}
}


std::string PatientEditMenu::getSelectedPatientFio() {
	std::vector<std::string> rec = m_patients.at(m_page_start + m_selected_row - 1);
	std::string first = Utils::wstring_to_utf8(Utils::utf8_to_wstring(rec.at(to_base(DbConnector::PatientList::FirstName))).substr(0, 1));
	std::string middle = Utils::wstring_to_utf8(Utils::utf8_to_wstring(rec.at(to_base(DbConnector::PatientList::MiddleName))).substr(0, 1));
	return rec.at(to_base(DbConnector::PatientList::LastName)) + " " + first + "." + middle + ".";
}

void PatientEditMenu::initData() {
	m_patients = ctx.m_db.getPatients(m_order_field, m_order_dir);
	m_selected_row = m_patients.size() ? 1 : 0;

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

	Widget::State st = m_patients.size() ? Widget::State::Passive : Widget::State::Disabled;
	m_delete->setState(st);
	m_edit->setState(st);
}
