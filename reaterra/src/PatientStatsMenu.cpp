#include "PatientStatsMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "PatientListMenu.h"
#include "WidgetBreadCrumbs.h"

PatientStatsMenu::PatientStatsMenu()
:	Menu {0, 0, "patient_stats_menu.png"}
{
	LD("Ctor");

	int id = stoi(ctx.m_prev_menu["PatientStatsMenu.params"].at(0));
	m_patient = ctx.m_db.getPatient(id);
	m_statistic = ctx.m_db.getStatistic(id);

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПАЦИЕНТЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());}, bc_link_style},
		{M("статистика по учетной записи пациента"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	m_set = addWidget(std::make_shared<WidgetImageButton>(1028, 666, "patient_list_menu_set_active.png", "patient_list_menu_set_disabled.png", "",
			[this, id](WidgetImageButton::Event e) {
				std::vector<std::string> rec = m_patient.at(0);

				setPatient(Utils::getFio(rec.at(to_base(DbConnector::PatientList::LastName)),
						rec.at(to_base(DbConnector::PatientList::FirstName)),
						rec.at(to_base(DbConnector::PatientList::MiddleName))),
						id,
						stof(rec.at(to_base(DbConnector::PatientList::Weight))),
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
	addWidget(std::make_shared<WidgetTextField>(120, 120, 543, 60, style, M("ФИО")));
	addWidget(std::make_shared<WidgetTextField>(120, 181, 543, 60, style, M("суммарное пройденное расстояние")));
	addWidget(std::make_shared<WidgetTextField>(120, 242, 543, 60, style, M("суммарное время процедур")));
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(750, 242, 343, 60, style, M("ч")));
	addWidget(std::make_shared<WidgetTextField>(880, 242, 343, 60, style, M("мин")));
	addWidget(std::make_shared<WidgetTextField>(1010, 242, 343, 60, style, M("с")));
	addWidget(std::make_shared<WidgetTextField>(760, 424, 543, 106, style, M("удар/мин")));
	addWidget(std::make_shared<WidgetTextField>(760, 532, 543, 106, style, M("удар/мин")));
	addWidget(std::make_shared<WidgetTextField>(830, 181, 543, 60, style, M(ctx.m_db.getParam("ЕдиницыРасстояние"))));
	addWidget(std::make_shared<WidgetTextField>(830, 303, 543, 60, style, M(ctx.m_db.getParam("ЕдиницыЭнергия"))));
	addWidget(std::make_shared<WidgetTextField>(830, 364, 543, 60, style, M("м/с2")));

	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(120, 303, 543, 60, style, M("суммарная потраченная энергия")));
	addWidget(std::make_shared<WidgetTextField>(120, 364, 543, 60, style, M("максимально достигнутое ускорение")));
	addWidget(std::make_shared<WidgetTextField>(120, 424, 543, 106, style, M("максимально достигнутое\\значение пульса")));
	addWidget(std::make_shared<WidgetTextField>(120, 532, 543, 106, style, M("минимально достигнутое\\значение пульса")));

	style.text_color_disabled = to_base(Colors::Black);
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(703, 58, 343, 60, style))->setText(m_patient.at(0).at(to_base(DbConnector::PatientList::Number)));

	std::vector<std::string> s = m_statistic.at(0);
	using Stat = DbConnector::Statistic;
	addWidget(std::make_shared<WidgetTextField>(703, 120, 343, 60, style, s.at(to_base(Stat::Fio))));
	addWidget(std::make_shared<WidgetTextField>(703, 181, 343, 60, style))
		->setText(s.at(to_base(Stat::Distance)).empty() ? "" : ctx.m_session.getDistanceString(std::stof(s.at(to_base(Stat::Distance)))));
	int duration = s.at(to_base(Stat::Duration)).empty() ? 0 : std::stoi(s.at(to_base(Stat::Duration)));
	addWidget(std::make_shared<WidgetTextField>(703, 242, 343, 60, style, std::to_string(duration / 3600)));
	addWidget(std::make_shared<WidgetTextField>(833, 242, 343, 60, style, std::to_string(duration % 3600 / 60)));
	addWidget(std::make_shared<WidgetTextField>(963, 242, 343, 60, style, std::to_string(duration % 60)));
	addWidget(std::make_shared<WidgetTextField>(703, 303, 343, 60, style))
		->setText(s.at(to_base(Stat::Energy)).empty() ? "" : ctx.m_session.getEnergyString(std::stof(s.at(to_base(Stat::Energy)))));
	addWidget(std::make_shared<WidgetTextField>(703, 364, 343, 60, style))
		->setText(s.at(to_base(Stat::Acceleration)).empty() ? "" : ctx.m_session.getAccelString(std::stof(s.at(to_base(Stat::Acceleration)))));
	addWidget(std::make_shared<WidgetTextField>(703, 424, 343, 106, style, s.at(to_base(Stat::HRateMax))));
	addWidget(std::make_shared<WidgetTextField>(703, 532, 343, 106, style, s.at(to_base(Stat::HRateMin))));

	style.font_size = 24;
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	style.font_style = FontStyle::Bold;
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(120, 58, 543, 60, style, M("Данные пациента №")));

	style.font_size = 26;


	style.text_color_disabled = to_base(Colors::White);
	style.font_style = FontStyle::Normal;
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(0, 653, 1006, 93, style))
		->setText(M("активация учетной записи"))
		.setState(Widget::State::Disabled);
}

PatientStatsMenu::~PatientStatsMenu() {
	LD("Dtor");
}
