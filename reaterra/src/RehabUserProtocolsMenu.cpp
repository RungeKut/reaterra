#include "RehabUserProtocolsMenu.h"

#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalAuthDoctor.h"
#include "ModalCommonDialog.h"
#include "PatientNewMenu.h"
#include "RehabMenu.h"
#include "RehabTestProtocolMenu.h"
#include "RehabProfileRunMenu.h"
#include "RehabSelProfileMenu.h"
#include "RehabUserProtocolsEditMenu.h"
#include "RehabUserProtocolsViewMenu.h"
#include "RehabSelMethodMenu.h"
#include "WidgetBreadCrumbs.h"

RehabUserProtocolsMenu::RehabUserProtocolsMenu()
:	Menu {0, 0, "rehab_user_protocols_menu.png"},
	m_selected_row {0},
	m_page_start {0},
	m_user_protocol {-1},
	m_first_update {true}
{
	LD("Ctor");
	m_pro_mode = to_enum<ProMode>(stoi(ctx.m_prev_menu["RehabUserProtocolsMenu.params"].at(0)));

	std::string bc_title {"пользовательские протоколы тестирования"};
	std::string subject_la = "протокола";
	std::string subject_lov = "протоколов";
	std::string subject_lu = "протоколу";
	std::string first_line = "тестирование\\по ";
	if (m_pro_mode != ProMode::Protocol) {
		if (m_pro_mode == ProMode::ProfilSelect) {
			bc_title = "список доступных беговых профилей";
		} else {
			bc_title = "пользовательские профили";
		}
		subject_la = "профиля";
		subject_lov = "профилей";
		subject_lu = "профилю";
		first_line = "бег по\\";
	}

	if (m_pro_mode == ProMode::ProfilSelect) {
		m_patients_btn->setState(Widget::State::Disabled);
		m_hr_btn->setState(Widget::State::Disabled);
		addBreadCrumps(std::vector<WBCElement> {
			{M(bc_title), WIBFunc(), bc_last_style}
		});
	} else {
		addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
			{M(bc_title), WIBFunc(), bc_last_style}
		});
	}

	addOnBackAction(std::function<std::unique_ptr<::Event> (WidgetImageButton::Event)> {[this](WidgetImageButton::Event e) {
		if (m_pro_mode == ProMode::ProfilSelect) {
			if (m_user_protocol != -1) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("ВНИМАНИЕ!\\ВЫХОД БЕЗ ВЫБОРА"),
						M("ОЧИСТИТЬ ПОЛЬЗОВАТЕЛЬСКИЙ\\ПРОФИЛЬ?"),
						"PSEL", Flags::GreenTitle | Flags::YesCancel, "icon_save_green.png");

				return std::unique_ptr<MenuEvent>(nullptr);
			} else {
				return std::make_unique<MenuEvent>(std::make_unique<PatientNewMenu>());
			}
		}
		return std::make_unique<MenuEvent>(std::make_unique<Menu>());
	}});

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = 0x003062;
	style.font_size = 23;
	style.font_style = FontStyle::Bold;

	addWidget(std::make_shared<WidgetTextField>(0, 46, 332, 48, style, M("название " + subject_la)));
	addWidget(std::make_shared<WidgetTextField>(333, 46, 884, 48, style, M("описание " + subject_la)));

	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Left;
	style.rect_color_active = to_base(Colors::None);;

	if (m_pro_mode != ProMode::ProfilSelect) {
		addWidget(std::make_shared<WidgetImageButton>(427, 642, "rehab_user_protocols_editor.png", "", "",
				[this](WidgetImageButton::Event e) {
					m_active_widget = nullptr;
					if (m_protocols.size()) {
						int id = getSelectedProtocolId();
						ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
					} else {
						ctx.m_prev_menu.erase("UserProfiles");
					}
					if (ctx.m_notuse_doc_password) {
						ctx.m_prev_menu["RehabUserProtocolsEditMenu.params"] = {std::to_string(to_base(m_pro_mode))};
						return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsEditMenu>());
					} else {
						m_modal = std::make_unique<ModalAuthDoctor>();
					}
					return std::make_unique<MenuEvent>(nullptr);
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(592, 642, 261, 104, style, M("редактор\\пользовательских\\" + subject_lov))));

		m_view = addWidget(std::make_shared<WidgetImageButton>(0, 642, "rehab_user_protocols_vew.png", "rehab_user_protocols_vew_disabled.png", "",
				[this](WidgetImageButton::Event e) {
					int id = getSelectedProtocolId();
					ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
					ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"] = {std::to_string(id),
							std::to_string(to_base(RehabUserProtocolsViewMenu::Mode::View)), std::to_string(to_base(m_pro_mode))};
					return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsViewMenu>());
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(169, 642, 256, 104, style, M("просмотр\\выбранного\\" + subject_la))));

		m_run = addWidget(std::make_shared<WidgetImageButton>(855, 642, "rehab_user_protocols_run.png", "rehab_user_protocols_run_disabled.png", "",
				[this](WidgetImageButton::Event e) {
					int id = getSelectedProtocolId();
					ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
					if (m_pro_mode == ProMode::Protocol) {
						ctx.m_prev_menu["RehabTestProtocolMenu.params"] = {std::to_string(id)};
						return std::make_unique<MenuEvent>(std::make_unique<RehabTestProtocolMenu>());
					} else {
						ctx.m_prev_menu["RehabProfileRunMenu.params"] = {std::to_string(id)};
						return std::make_unique<MenuEvent>(std::make_unique<RehabProfileRunMenu>());
					}
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(1047, 642, 233, 104, style, M(first_line + "выбранному\\" + subject_lu))));

	} else {
		addWidget(std::make_shared<WidgetImageButton>(0, 642, "", "rehab_user_protocols_select.png"))->setState(Widget::State::Disabled);
		m_save = addWidget(std::make_shared<WidgetImageButton>(557, 652, "rehab_user_protocols_select_active.png", "rehab_user_protocols_select_disabled.png", "",
				[this](WidgetImageButton::Event e) {
					PatientNewMenu::setProfile(std::to_string(getSelectedProtocolId()), m_protocols.at(m_page_start + m_selected_row - 1).at(to_base(DbConnector::ProtocolList::Name)));
					return std::make_unique<MenuEvent>(std::make_unique<PatientNewMenu>());
				}));
		style.halign = Align::Right;
		style.text_color_disabled = to_base(Colors::White);
		style.font_size = 28;
		addWidget(std::make_shared<WidgetTextField>(0, 642, 538, 104, style, M("ВЫБРАТЬ")));
	}

	m_pgup = addWidget(std::make_shared<WidgetImageButton>(1225, 54, "rehab_user_protocols_pgup_active.png", "", "rehab_user_protocols_pgup_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start -= TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_pgdn = addWidget(std::make_shared<WidgetImageButton>(1225, 589, "rehab_user_protocols_pgdn_active.png", "", "rehab_user_protocols_pgdn_passive.png",
			[this](WidgetImageButton::Event e) {
				m_page_start += TABLE_ROWS;
				m_selected_row = 1;
				updateTable();
				return nullptr;
			}));

	m_thumb = addWidget(std::make_shared<WidgetRectangle>(1226, 0, 44, 10, 0xBEBFBF));

	int cols[] = {0, 332, 1217};

	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		m_list.push_back(std::make_shared<WidgetImageButton>(0, ROW_START_Y + i * ROW_STEP_Y, "rehab_user_protocols_" + std::to_string(i + 1) + ".png", "", "",
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
		m_text_list.back().push_back(std::make_shared<WidgetText>(cols[0] + (cols[1] - cols[0]) / 2, ROW_START_Y + (i + 0.5) * ROW_STEP_Y, "",
				Align::Center, 25, 0x0, 0x0, std::function<std::string(void)>{}, FontStyle::Normal, (cols[1] - cols[0]), 2));
		addWidget(m_text_list.back().at(0));
		m_text_list.back().push_back(std::make_shared<WidgetText>(cols[1] + (cols[2] - cols[1]) / 2, ROW_START_Y + (i + 0.5) * ROW_STEP_Y, "",
				Align::Center, 25, 0x0, 0x0, std::function<std::string(void)>{}, FontStyle::Normal, (cols[2] - cols[1]), 2));
		addWidget(m_text_list.back().at(1));
	}
}

RehabUserProtocolsMenu::~RehabUserProtocolsMenu() {
	LD("Dtor");
}

void RehabUserProtocolsMenu::updateTable() {
	unsigned active = m_protocols.size() - m_page_start;

	for (unsigned i = 0; i < m_list.size(); ++i) {
		if (i < active) {
			m_list.at(i)->setState(Widget::State::Passive);
			m_text_list.at(i).at(0)->setText(m_protocols.at(m_page_start + i).at(to_base(DbConnector::ProtocolList::Name)));
			m_text_list.at(i).at(1)->setText(m_protocols.at(m_page_start + i).at(to_base(DbConnector::ProtocolList::Description)));
		} else {
			m_list.at(i)->setState(Widget::State::Disabled);
			m_text_list.at(i).at(0)->setText("");
			m_text_list.at(i).at(1)->setText("");
		}
		m_list.at(i)->clearTrigger();
	}

	if (m_selected_row) {
		m_list.at(m_selected_row - 1)->setTrigger();
	}

	if (m_protocols.size() - m_page_start > TABLE_ROWS) {
		m_pgdn->setState(Widget::State::Passive);
	} else {
		m_pgdn->setState(Widget::State::Disabled);
	}
	if (m_page_start) {
		m_pgup->setState(Widget::State::Passive);
	} else {
		m_pgup->setState(Widget::State::Disabled);
	}
	if (m_protocols.size() > TABLE_ROWS) {
		m_thumb->setY(102 + 482. * m_page_start / m_protocols.size());
		m_thumb->setHeight(482. * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_protocols.size());
		m_thumb->setVisible(true);
	}
}

int RehabUserProtocolsMenu::getSelectedProtocolId() {
	return std::stoi(m_protocols.at(m_page_start + m_selected_row - 1).at(to_base(DbConnector::ProtocolList::Id)));
}

Menu* RehabUserProtocolsMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	if (m_first_update) {
		m_first_update = false;
		initData();
		updateTable();
	}

	if (m_modal && m_modal->getType() == "AUDO"
			&& (event->getType() == Event::EventType::ModalYes || event->getType() == Event::EventType::ModalCancel)) {
		if (event->getType() == Event::EventType::ModalYes) {
			//TODO: избавиться от new при передаче меню
			ctx.m_prev_menu["RehabUserProtocolsEditMenu.params"] = {std::to_string(to_base(m_pro_mode))};
			ret = new RehabUserProtocolsEditMenu();
		}
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	} else if (m_modal && m_modal->getType() == "PSEL"
			&& (event->getType() == Event::EventType::ModalYes || event->getType() == Event::EventType::ModalCancel)) {
		if (event->getType() == Event::EventType::ModalYes) {
			PatientNewMenu::setProfile("0", "");
		}
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
//		ret = new PatientNewMenu(-1, PatientNewMenu::Mode::New);
		ret = new PatientNewMenu();
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}

void RehabUserProtocolsMenu::initData() {
	m_protocols = ctx.m_db.getProtocols(m_pro_mode == ProMode::Protocol ? "USER" : "USER_PROF");
	m_selected_row = m_protocols.size() ? 1 : 0;

	if (m_pro_mode == ProMode::ProfilSelect) {
		Widget::State st = m_protocols.size() ? Widget::State::Passive : Widget::State::Disabled;
		m_save->setState(st);
		std::string pr = PatientNewMenu::getProfile();
		if (!pr.empty()) {
			for (size_t i = 0; i < m_protocols.size(); ++i) {
				if (m_protocols.at(i).at(to_base(DbConnector::ProtocolList::Name)) == pr) {
					m_selected_row = i + 1;
					m_user_protocol = m_selected_row;
					break;
				}
			}
		}
	}

	if (ctx.m_prev_menu.find("UserProfiles") != ctx.m_prev_menu.end() && m_pro_mode != ProMode::ProfilSelect) {
		if (m_protocols.size()) {
			m_selected_row = 1;
		}

		for (size_t i = 0; i < m_protocols.size(); ++i) {
			if (m_protocols.at(i).at(to_base(DbConnector::ProtocolList::Id)) == ctx.m_prev_menu["UserProfiles"].at(1)) {
				m_selected_row = i + 1;
				break;
			}
		}
	}

	while (m_selected_row > TABLE_ROWS) {
		m_selected_row -= TABLE_ROWS;
		m_page_start += TABLE_ROWS;
	}

	if (m_protocols.size() == 0 && m_pro_mode != ProMode::ProfilSelect) {
		m_view->setState(Widget::State::Disabled);
		m_run->setState(Widget::State::Disabled);
	}
}
