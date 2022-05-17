#include "RehabUserProtocolsEditMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "ModalCommonDialog.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "RehabUserProtocolsViewMenu.h"
#include "WidgetBreadCrumbs.h"

RehabUserProtocolsEditMenu::RehabUserProtocolsEditMenu()
:	Menu {0, 0, "rehab_user_protocols_edit_menu.png"},
	m_selected_row {0},
	m_page_start {0},
	m_first_update {true}
{
	LD("Ctor");

	m_pro_mode = to_enum<RehabUserProtocolsMenu::ProMode>(stoi(ctx.m_prev_menu["RehabUserProtocolsEditMenu.params"].at(0)));

	std::string subject_l = "протокол";
	std::string subject_l_uc = "ПРОТОКОЛ";
	std::string subject_la = "протокола";
	std::string subject_lov = "протоколов";
	std::string subject_lu = "протоколу";
	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		subject_l = "профиль";
		subject_l_uc = "ПРОФИЛЬ";
		subject_la = "профиля";
		subject_lov = "профилей";
		subject_lu = "профилю";
	}

	if (m_pro_mode == RehabUserProtocolsMenu::ProMode::Profil) {
		addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
			{M("ПРОФИЛИ"), [this](WidgetImageButton::Event e){
					ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Profil))};
					return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsMenu>());
				}, bc_link_style},
			{M("редактирование " + subject_lov), WIBFunc(), bc_last_style}
		});
	} else {
		addBreadCrumps(std::vector<WBCElement> {
			{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
			{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
			{M("ПРОТОКОЛЫ"), [this](WidgetImageButton::Event e){
					ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Protocol))};
					return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsMenu>());
				}, bc_link_style},
			{M("редактирование " + subject_lov), WIBFunc(), bc_last_style}
		});

	}

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.text_color_disabled = 0x003062;
	style.font_size = 23;
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(0, 123, 332, 48, style, M("название " + subject_la)));
	addWidget(std::make_shared<WidgetTextField>(333, 123, 884, 48, style, M("описание " + subject_la)));

	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(146, 46, 1134, 77, style, M("Редактирование выбранного " + subject_la + " тестирования")));

	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Left;
	style.rect_color_active = to_base(Colors::None);;

	addWidget(std::make_shared<WidgetImageButton>(0, 642, "rehab_user_protocols_edit_add_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_active_widget = nullptr;
				if (m_protocols.size()) {
					int id = getSelectedProtocolId();
					ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
				}
				ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"] = {std::to_string(-1),
						std::to_string(to_base(RehabUserProtocolsViewMenu::Mode::New)), std::to_string(to_base(m_pro_mode))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsViewMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(169, 642, 256, 104, style, M("добавить\\новый\\" + subject_l))));

	m_modify = addWidget(std::make_shared<WidgetImageButton>(427, 642, "rehab_user_protocols_edit_modify_active.png", "rehab_user_protocols_edit_modify_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				int id = getSelectedProtocolId();
				ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", std::to_string(id), std::to_string(m_page_start), std::to_string(m_selected_row)};
				ctx.m_prev_menu["RehabUserProtocolsViewMenu.params"] = {std::to_string(id),
						std::to_string(to_base(RehabUserProtocolsViewMenu::Mode::Edit)), std::to_string(to_base(m_pro_mode))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsViewMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(592, 642, 261, 104, style, M("редактировать\\выбранный\\" + subject_l))));

	m_delete = addWidget(std::make_shared<WidgetImageButton>(855, 642, "rehab_user_protocols_edit_delete_active.png", "rehab_user_protocols_edit_delete_disabled.png", "",
			[this, subject_l_uc](WidgetImageButton::Event e) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("ВНИМАНИЕ!\\УДАЛИТЬ " + subject_l_uc + "?"),
						m_protocols.at(m_page_start + m_selected_row - 1).at(static_cast<int>(DbConnector::ProtocolList::Name)),
						"PDEL", Flags::YellowTitle | Flags::YesCancel, "icon_protocol_cross_yellow.png");
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(1027, 642, 253, 104, style, M("удалить\\выбранный\\" + subject_l))));

	m_pgup = addWidget(std::make_shared<WidgetImageButton>(1225, 131, "rehab_user_protocols_pgup_active.png", "", "rehab_user_protocols_pgup_passive.png",
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
		m_list.push_back(std::make_shared<WidgetImageButton>(0, ROW_START_Y + i * ROW_STEP_Y, "rehab_user_protocols_edit_" + std::to_string(i + 1) + ".png", "", "",
				[this, i](WidgetImageButton::Event e) {
					for (unsigned k = 0; k < m_list.size(); ++k) {
						if (k != i) {
							m_list.at(k)->clearTrigger();
						}
					}
					m_selected_row = i + 1;
//					LD("Selected: " + std::to_string(m_selected_row));
					return nullptr;
			}, WidgetImageButton::Type::Trigger));
		addWidget(m_list.back());

		m_text_list.push_back({});
		m_text_list.back().push_back(std::make_shared<WidgetTextField>(cols[0], ROW_START_Y + i * ROW_STEP_Y - 1,
				cols[1] - cols[0], ROW_STEP_Y - 1, rehab_user_protocol_table_style));
		addWidget(m_text_list.back().at(0));
		m_text_list.back().push_back(std::make_shared<WidgetTextField>(cols[1], ROW_START_Y + i * ROW_STEP_Y - 1,
				cols[2] - cols[1], ROW_STEP_Y - 1, rehab_user_protocol_table_style));
		addWidget(m_text_list.back().at(1));
	}
}

RehabUserProtocolsEditMenu::~RehabUserProtocolsEditMenu() {
	LD("Dtor");
}


void RehabUserProtocolsEditMenu::updateTable() {
	unsigned active = m_protocols.size() - m_page_start;

	for (unsigned i = 0; i < m_list.size(); ++i) {
		if (i < active) {
			m_list.at(i)->setState(Widget::State::Passive);
			m_text_list.at(i).at(0)->setText(m_protocols.at(m_page_start + i).at(static_cast<int>(DbConnector::ProtocolList::Name)));
			m_text_list.at(i).at(1)->setText(m_protocols.at(m_page_start + i).at(static_cast<int>(DbConnector::ProtocolList::Description)));
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
		m_thumb->setY(179 + 405. * m_page_start / m_protocols.size());
		m_thumb->setHeight(405. * (active < TABLE_ROWS ? active : TABLE_ROWS) / m_protocols.size());
		if (m_thumb->getHeight() < 20) {
			m_thumb->setY(179 + 405. - 20);
			m_thumb->setHeight(20);
		}
		m_thumb->setVisible(true);
	} else {
		m_thumb->setVisible(false);
	}

	if (m_protocols.size() == 0) {
		m_modify->setState(Widget::State::Disabled);
		m_delete->setState(Widget::State::Disabled);
	} else {
		m_modify->setState(Widget::State::Passive);
		m_delete->setState(Widget::State::Passive);
	}
}


int RehabUserProtocolsEditMenu::getSelectedProtocolId() {
	return std::stoi(m_protocols.at(m_page_start + m_selected_row - 1).at(static_cast<int>(DbConnector::ProtocolList::Id)));
}

Menu* RehabUserProtocolsEditMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	if (m_first_update) {
		m_first_update = false;
		initData();
		updateTable();
	}

	if (m_modal && m_modal->getType() == "PDEL" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();

		int id = getSelectedProtocolId();
		ctx.m_db.deleteProtocol(id);
		if (ctx.m_profile_id == id) {
			ctx.m_profile_id = 0;
		}
		int index = m_page_start + m_selected_row - 1;

		if (index == 0 && m_protocols.size() == 1) {
			m_selected_row = 0;
			ctx.m_prev_menu.erase("UserProfiles");
			m_protocols.clear();
		} else {
			if (index == 0) {
				index++;
			} else {
				index--;
			}
			std::string new_id = m_protocols.at(index).at(static_cast<int>(DbConnector::ProtocolList::Id));
			m_protocols = ctx.m_db.getProtocols(m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol ? "USER" : "USER_PROF");
			recalculatePage(new_id);
			ctx.m_prev_menu["UserProfiles"] = {"UserProfiles", new_id, std::to_string(m_page_start), std::to_string(m_selected_row)};
		}
		updateTable();
	} else if (event->getType() == Event::EventType::ModalCancel) {
		m_modal.reset();
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	return ret;
}

void RehabUserProtocolsEditMenu::recalculatePage(std::string id) {
	m_page_start = 0;
	if (!m_protocols.size()) {
		m_selected_row = 0;
		return;
	}

	for (unsigned i = 0; i < m_protocols.size(); ++i) {
		if (m_protocols.at(i).at(static_cast<int>(DbConnector::ProtocolList::Id)) == id) {
			m_selected_row = i + 1;
			break;
		}
	}

	while (m_selected_row > TABLE_ROWS) {
		m_selected_row -= TABLE_ROWS;
		m_page_start += TABLE_ROWS;
	}
}

void RehabUserProtocolsEditMenu::initData() {
	m_protocols = ctx.m_db.getProtocols(m_pro_mode == RehabUserProtocolsMenu::ProMode::Protocol ? "USER" : "USER_PROF");
	m_selected_row = m_protocols.size() ? 1 : 0;

	if (ctx.m_prev_menu.find("UserProfiles") != ctx.m_prev_menu.end()) {
		recalculatePage(ctx.m_prev_menu["UserProfiles"].at(1));
	}
}
