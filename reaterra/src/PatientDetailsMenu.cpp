#include "PatientDetailsMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "PatientListMenu.h"
#include "WidgetBreadCrumbs.h"

PatientDetailsMenu::PatientDetailsMenu()
:	Menu {0, 0, "patient_details_menu.png"}
{
	LD("Ctor");

	int id = 0;
	if (ctx.m_prev_menu.find("PatientList") != ctx.m_prev_menu.end()) {
		id = stoi(ctx.m_prev_menu["PatientList"].at(1));
		m_patient = ctx.m_db.getPatient(id);
	}

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M("детальная информация о пациенте"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	m_set = addWidget(std::make_shared<WidgetImageButton>(1028, 666, "patient_list_menu_set_active.png", "patient_list_menu_set_disabled.png", "",
			[this, id](WidgetImageButton::Event e) {
				std::vector<std::string> rec = m_patient.at(0);

				setPatient(Utils::getFio(rec.at(to_base(DbConnector::PatientList::LastName)),
						rec.at(to_base(DbConnector::PatientList::FirstName)),
						rec.at(to_base(DbConnector::PatientList::MiddleName))),
						id, stof(rec.at(to_base(DbConnector::PatientList::Weight))),
						stoi(rec.at(to_base(DbConnector::PatientList::ProfileId))));
				m_unset->setState(Widget::State::Passive);
				return nullptr;
			}));
	m_unset = addWidget(std::make_shared<WidgetImageButton>(1149, 666, "patient_list_menu_unset_active.png", "patient_list_menu_unset_disabled.png", "",
			[this](WidgetImageButton::Event e) {
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
	style.lines = 6;
	style.font_size = 26;
	style.rect_color_active = to_base(Colors::None);
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(137, 120, 343, 60, style, M("фамилия")));
	addWidget(std::make_shared<WidgetTextField>(137, 181, 343, 60, style, M("имя")));
	addWidget(std::make_shared<WidgetTextField>(137, 242, 343, 60, style, M("отчество")));
	addWidget(std::make_shared<WidgetTextField>(137, 303, 343, 60, style, M("возраст")));
	addWidget(std::make_shared<WidgetTextField>(137, 364, 343, 60, style, M("лечащий врач")));
	addWidget(std::make_shared<WidgetTextField>(374, 303, 343, 60, style, M("пол")));
	addWidget(std::make_shared<WidgetTextField>(614, 303, 343, 60, style, M("вес")));
	style.valign = Align::Top;
	addWidget(std::make_shared<WidgetTextField>(137, 424 + 20, 343, 129, style, M("диагноз")));
	addWidget(std::make_shared<WidgetTextField>(137, 554 + 20, 343, 99, style, M("профиль")));

	style.valign = Align::Center;
	style.text_color_disabled = to_base(Colors::Black);
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(503, 58, 343, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Number))));
	addWidget(std::make_shared<WidgetTextField>(503, 120, 603, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::LastName))));
	addWidget(std::make_shared<WidgetTextField>(503, 181, 603, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::FirstName))));
	addWidget(std::make_shared<WidgetTextField>(503, 242, 603, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::MiddleName))));
	addWidget(std::make_shared<WidgetTextField>(503, 303, 343, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Age))));
	addWidget(std::make_shared<WidgetTextField>(503, 364, 603, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Doctor))));
	addWidget(std::make_shared<WidgetTextField>(740, 303, 343, 60, style, M(m_patient.at(0).at(to_base(DbConnector::PatientList::Sex)))));
	addWidget(std::make_shared<WidgetTextField>(980, 303, 343, 60, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Weight))));
	style.valign = Align::Top;
	addWidget(std::make_shared<WidgetTextField>(503, 424 + 20, 603, 129, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Diagnosis))));
	addWidget(std::make_shared<WidgetTextField>(503, 554 + 20, 603, 99, style, m_patient.at(0).at(to_base(DbConnector::PatientList::Profile))));
	style.valign = Align::Center;

	style.font_size = 24;
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	style.font_style = FontStyle::Bold;
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(137, 58, 343, 60, style, M("Данные пациента №")));

	style.font_size = 26;
	style.text_color_disabled = to_base(Colors::White);
	style.font_style = FontStyle::Normal;
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(0, 653, 1006, 93, style))
		->setText(M("активация учетной записи"))
		.setState(Widget::State::Disabled);
}

PatientDetailsMenu::~PatientDetailsMenu() {
	LD("Dtor");
}
