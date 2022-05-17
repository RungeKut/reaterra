#include "PatientNewMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "ModalCommonDialog.h"
#include "PatientEditMenu.h"
#include "PatientListMenu.h"
#include "RehabMenu.h"
#include "RehabUserProfilSelectMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "Utils.h"

PatientNewMenu::PatientNewMenu()
:	Menu {0, 0, "patient_new_menu.png"},
	m_changed {false}
{
	LD("Ctor");

	m_patient = ctx.m_db.getPatient(stoi(ctx.m_prev_menu["PatientNewMenu.params"].at(1)));
	m_mode = to_enum<Mode>(stoi(ctx.m_prev_menu["PatientNewMenu.params"].at(2)));

	if (ctx.m_prev_menu.find("PatientState") != ctx.m_prev_menu.end()) {
		restoreState();
		ctx.m_prev_menu.erase("PatientState");
		m_changed = true;
	}

	LD("id:" + m_patient.at(0).at(to_base(DbConnector::PatientList::Id)));

	std::string title;
	switch (m_mode) {
	case Mode::New:
		title = "новая учетная запись пациента";
		break;
	case Mode::Edit:
		title = "редактирование данных пациента";
		m_prev_fio = m_patient.at(0).at(to_base(DbConnector::PatientList::LastName));
		break;
	}

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return backAction(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return backAction(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M(title), WIBFunc(), bc_last_style}
	});

	addOnBackAction(std::function<std::unique_ptr<::Event> (WidgetImageButton::Event)> {[this](WidgetImageButton::Event e) {
		return backAction(std::make_unique<PatientEditMenu>());
	}});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.halign = Align::Left;
	style.text_color_disabled = to_base(Colors::White);
	style.font_size = 22;
	style.font_style = FontStyle::Normal;
	addWidget(std::make_shared<WidgetTextField>(57, 137, 75, 39, style, M("№")));
	addWidget(std::make_shared<WidgetTextField>(189, 137, 303, 39, style, M("фамилия")));
	addWidget(std::make_shared<WidgetTextField>(549, 137, 285, 39, style, M("имя")));
	addWidget(std::make_shared<WidgetTextField>(892, 137, 331, 39, style, M("отчество")));
	addWidget(std::make_shared<WidgetTextField>(755, 285, 150, 39, style, M("возраст")));
	addWidget(std::make_shared<WidgetTextField>(916, 285, 150, 39, style, M("вес")));
	addWidget(std::make_shared<WidgetTextField>(57, 240, 649, 39, style, M("диагноз")));
	addWidget(std::make_shared<WidgetTextField>(57, 432, 649, 39, style, M("лечащий врач")));
	addWidget(std::make_shared<WidgetTextField>(57, 535, 649, 39, style, M("беговой профиль")));
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(1095, 265, 91, 39, style, M("пол")));
	addWidget(std::make_shared<WidgetTextField>(0, 662, 1280, 84, style, M("ДЛЯ ВЫБОРА БЕГОВОГО ПРОФИЛЯ НАЖМИТЕ СООТВЕТСТВУЮЩУЮ КНОПКУ")));
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(716, 569, 309, 98, style, M("ПОЛЬЗОВАТЕЛЬСКИЕ\\БЕГОВЫЕ ПРОФИЛИ")));
	style.text_color_disabled = 0x003264;
	style.halign = Align::Center;
	style.font_size = 26;
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(119, 47, 1161, 76, style, M("Данные пациента")));

	addWidget(std::make_shared<WidgetImageButton>(1041, 568, "patient_new_menu_profile_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				saveState();
				ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::ProfilSelect))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProfilSelectMenu>());
			}));

	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.halign = Align::Center;
	style.font_size = 40;
	style.rect_color_active = to_base(Colors::None);
	m_male = addWidget(std::make_shared<WidgetImageButton>(1057, 304, "patient_new_menu_male_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_female->clearTrigger();
				m_patient.at(0).at(to_base(DbConnector::PatientList::Sex)) = "М";
				m_changed = true;
				return nullptr;
			}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(1060, 306, 93, 82, style, M("М"))));
	m_female = addWidget(std::make_shared<WidgetImageButton>(1139, 304, "patient_new_menu_female_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_male->clearTrigger();
				m_patient.at(0).at(to_base(DbConnector::PatientList::Sex)) = "Ж";
				m_changed = true;
				return nullptr;
			}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(1129, 306, 93, 82, style, M("Ж"))));

	if (m_patient.at(0).at(to_base(DbConnector::PatientList::Sex)) == "М") {
		m_male->setTrigger();
	} else {
		m_female->setTrigger();
	}

	style.kbd_type = KbdType::Alpha;
	style.rect_color_active = 0x87CEEA;
	style.text_color_passive = to_base(Colors::Black);
	style.text_color_active = style.text_color_passive;
	style.text_color_disabled = style.text_color_passive;
	style.halign = Align::Center;
	style.lines = 1;
	style.font_size = 28;
	using Pat = DbConnector::PatientList;
	addWidget(std::make_shared<WidgetTextField>(57, 176, 75, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::Number))); // Идентификатор
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(57, 574, 649, 88, style))
		->setText(m_patient.at(0).at(to_base(Pat::Profile))); // Профиль
	style.check_func = [this](std::string s, WidgetTextField &w) {
		m_patient.at(0).at(std::stoi(w.getTextId())) = s;
		m_changed = true;
		return "";
	};

	style.text_max_size = 20;
	addWidget(std::make_shared<WidgetTextField>(189, 176, 303, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::LastName)), std::to_string(to_base(Pat::LastName))) // Фамилия
		.setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(549, 176, 285, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::FirstName)), std::to_string(to_base(Pat::FirstName))) // Имя
		.setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(892, 176, 331, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::MiddleName)), std::to_string(to_base(Pat::MiddleName))) // Отчество
		.setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(57, 471, 649, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::Doctor)), std::to_string(to_base(Pat::Doctor))) // Врач
		.setState(Widget::State::Passive);
	style.lines = 3;
	style.text_max_size = 40;
	addWidget(std::make_shared<WidgetTextField>(57, 279, 649, 136, style))
		->setText(m_patient.at(0).at(to_base(Pat::Diagnosis)), std::to_string(to_base(Pat::Diagnosis))) // Диагноз
		.setState(Widget::State::Passive);
	style.kbd_type = KbdType::Digital;
	style.halign = Align::Center;
	style.text_max_size = 3;
	addWidget(std::make_shared<WidgetTextField>(755, 324, 112, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::Age)), std::to_string(to_base(Pat::Age))) // Возраст
		.setLimits(1, 999)
		.setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(916, 324, 92, 46, style))
		->setText(m_patient.at(0).at(to_base(Pat::Weight)), std::to_string(to_base(Pat::Weight))) // Вес
		.setLimits(1, 999)
		.setState(Widget::State::Passive);
}

PatientNewMenu::~PatientNewMenu() {
	LD("Dtor");
}

Menu* PatientNewMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	LD("Event enter" + event->getName());

	if (m_modal && m_modal->getType() == "SAVE" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();

		if (m_patient.at(0).at(to_base(DbConnector::PatientList::LastName)).empty()) {
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::make_unique<ModalCommonDialog>(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
					M("ВВЕДИТЕ ФАМИЛИЮ"), "SVER", Flags::YellowTitle | Flags::CloseButton,
					"icon_save_cross_yellow.png");
		} else if (m_patient.at(0).at(to_base(DbConnector::PatientList::Age)).empty()) {
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::make_unique<ModalCommonDialog>(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
					M("ВВЕДИТЕ ВОЗРАСТ"), "SVER", Flags::YellowTitle | Flags::CloseButton,
					"icon_save_cross_yellow.png");
		} else if (m_patient.at(0).at(to_base(DbConnector::PatientList::Weight)).empty()) {
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::make_unique<ModalCommonDialog>(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
					M("ВВЕДИТЕ ВЕС"), "SVER", Flags::YellowTitle | Flags::CloseButton,
					"icon_save_cross_yellow.png");
		} else {

			bool duplicated = false;

			//TODO: проверка на дубликат пациента
//			if (m_mode == Mode::New || (m_mode == Mode::Edit && m_prev_fio != m_patient.at(0).at(to_base(DbConnector::PatientList::LastName)))) {
//				DataSet protocols = ctx.m_db.getProtocols("USER");
//				for (const auto &p : protocols) {
//					if (p.at(static_cast<int>(DbConnector::ProtocolList::Name)) == m_patient.at(0).at(static_cast<int>(DbConnector::ProtocolParam::Name))) {
//						duplicated = true;
//						break;
//					}
//				}
//			}


			if (duplicated) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("ОШИБКА!\\СОХРАНЕНИЕ НЕВОЗМОЖНО"),
						M("ПАЦИЕНТ С ТАКИМ ФИО\\УЖЕ СУЩЕСТВУЕТ"), "SVDB", Flags::YellowTitle | Flags::CloseButton,
						"icon_protocol_exclamation_yellow.png");
			} else {

				if (m_mode == Mode::Edit) {
					ctx.m_db.updatePatient(m_patient);
					int id = std::stoi(m_patient.at(0).at(to_base(DbConnector::PatientList::Id)));
					if (ctx.m_patient_id == id) {
						setPatient(Utils::getFio(m_patient.at(0).at(to_base(DbConnector::PatientList::LastName)),
								m_patient.at(0).at(to_base(DbConnector::PatientList::FirstName)),
								m_patient.at(0).at(to_base(DbConnector::PatientList::MiddleName))),
								id, stof(m_patient.at(0).at(to_base(DbConnector::PatientList::Weight))),
								stoi(m_patient.at(0).at(to_base(DbConnector::PatientList::ProfileId))));
					}
				} else {
					ctx.m_prev_menu["PatientList"] = {"PatientList", std::to_string(ctx.m_db.insertPatient((m_patient)))};
				}
					ret = m_next_menu.release();
				}
		}
	} else if (m_modal && m_modal->getType() == "SAVE" && event->getType() == Event::EventType::ModalNo) {
		m_modal.reset();
		ret = m_next_menu.release();
	} else if (m_modal && m_modal->getType() == "SVER" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
	} else if (m_modal && m_modal->getType() == "SAVE" && event->getType() == Event::EventType::ModalCancel) {
		m_modal.reset();
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	LD("Event processed");
	return ret;
}

void PatientNewMenu::saveState() {
	std::vector<std::string> state;

	state.push_back("PatientState");
	state.push_back(m_mode == Mode::Edit ? "Edit" : "View");
	state.insert(state.end(), m_patient.at(0).begin(), m_patient.at(0).end());

	ctx.m_prev_menu["PatientState"] = state;
}

void PatientNewMenu::restoreState() {
	m_mode = ctx.m_prev_menu["PatientState"].at(1) == "Edit" ? Mode::Edit : Mode::New;
	m_patient.at(0).assign(ctx.m_prev_menu["PatientState"].begin() + 2, ctx.m_prev_menu["PatientState"].end());
}

void PatientNewMenu::setProfile(std::string profile_id, std::string profile_name) {
	if (ctx.m_prev_menu.find("PatientState") != ctx.m_prev_menu.end()) {
		ctx.m_prev_menu["PatientState"].at(to_base(DbConnector::PatientList::ProfileId) + 2) = profile_id;
		ctx.m_prev_menu["PatientState"].at(to_base(DbConnector::PatientList::Profile) + 2) = profile_name;
	}
}

std::string PatientNewMenu::getProfile() {
	if (ctx.m_prev_menu.find("PatientState") != ctx.m_prev_menu.end()) {
		return ctx.m_prev_menu["PatientState"].at(to_base(DbConnector::PatientList::Profile) + 2);
	} else {
		return "";
	}
}

std::unique_ptr<MenuEvent> PatientNewMenu::backAction(std::unique_ptr<Menu> menu) {
	std::unique_ptr<MenuEvent> ret = nullptr;
	if (m_changed) {
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::make_unique<ModalCommonDialog>(M("СОХРАНЕНИЕ ИЗМЕНЕНИЙ"), M("СОХРАНИТЬ ДАННЫЕ ПАЦИЕНТА?"), "SAVE",
				Flags::GreenTitle | Flags::YesNoCancel, "icon_save_green.png");
		m_next_menu = std::move(menu);
	} else {
		ret = std::make_unique<MenuEvent>(std::move(menu));
	}
	return ret;
}
