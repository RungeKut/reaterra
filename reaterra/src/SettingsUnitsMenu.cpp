#include "SettingsUnitsMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"
#include "WidgetBreadCrumbs.h"

SettingsUnitsMenu::SettingsUnitsMenu()
:	Menu {0, 0, "settings_units_menu.png", {}},
	m_dist_selected {ctx.m_db.getParam("ЕдиницыРасстояние")},
	m_speed_selected {ctx.m_db.getParam("ЕдиницыСкорость")},
	m_angle_selected {ctx.m_db.getParam("ЕдиницыУгол")},
	m_energy_selected {ctx.m_db.getParam("ЕдиницыЭнергия")},
	m_pwr_selected {ctx.m_db.getParam("ЕдиницыМощность")},
	m_pwrair_selected {ctx.m_db.getParam("ЕдиницыМощностьАэробная")}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("единицы измерения физических величин"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::Black);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = to_base(Colors::Black);
	style.halign = Align::Center;
	style.lines = 6;
	style.hpad = 0;
	style.font_size = 28;
	style.rect_color_active = to_base(Colors::None);

	addWidget(std::make_shared<WidgetTextField>(20, 92, 294, 60, style, M("РАССТОЯНИЕ")));
	addWidget(std::make_shared<WidgetTextField>(335, 92, 294, 60, style, M("СКОРОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(650, 92, 294, 60, style, M("УГОЛ")));
	addWidget(std::make_shared<WidgetTextField>(965, 92, 294, 60, style, M("ЭНЕРГИЯ")));
	addWidget(std::make_shared<WidgetTextField>(335, 406, 212, 60, style, M("МОЩНОСТЬ")));
	addWidget(std::make_shared<WidgetTextField>(547, 406, 398, 60, style, M("АЭРОБНАЯ")));
	style.font_size = 24;



	style.halign = Align::Left;
	style.font_style = FontStyle::Bold;
	m_dist_km = std::make_shared<WidgetImageButton>(22, 152, "settings_units_km.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_dist_m->clearTrigger();
				m_dist_mile->clearTrigger();
				m_dist_selected = "км";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(90, 150, 224, 51, style, M("км")));
	addWidget(m_dist_km);

	m_dist_m = std::make_shared<WidgetImageButton>(22, 231, "settings_units_m.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_dist_km->clearTrigger();
				m_dist_mile->clearTrigger();
				m_dist_selected = "м";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(90, 228, 224, 51, style, M("м")));
	addWidget(m_dist_m);

	m_dist_mile = std::make_shared<WidgetImageButton>(22, 309, "settings_units_mile.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_dist_km->clearTrigger();
				m_dist_m->clearTrigger();
				m_dist_selected = "мили";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(90, 306, 224, 51, style, M("миля")));
	addWidget(m_dist_mile);

	m_speed_kmh = std::make_shared<WidgetImageButton>(337, 152, "settings_units_kmh.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_speed_ms->clearTrigger();
				m_speed_mh->clearTrigger();
				m_speed_selected = "км/ч";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(396, 150, 234, 51, style, M("км/ч")));
	addWidget(m_speed_kmh);

	m_speed_ms = std::make_shared<WidgetImageButton>(337, 231, "settings_units_ms.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_speed_kmh->clearTrigger();
				m_speed_mh->clearTrigger();
				m_speed_selected = "м/с";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(396, 228, 234, 51, style, M("м/с")));
	addWidget(m_speed_ms);

	m_speed_mh = std::make_shared<WidgetImageButton>(337, 309, "settings_units_mh.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_speed_ms->clearTrigger();
				m_speed_kmh->clearTrigger();
				m_speed_selected = "миля/ч";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(396, 306, 234, 51, style, M("миля/ч")));
	addWidget(m_speed_mh);

	m_angle_gr = std::make_shared<WidgetImageButton>(652, 152, "settings_units_gr.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_angle_per->clearTrigger();
				m_angle_selected = "град";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(719, 170, 225, 51, style, M("град")));
	addWidget(m_angle_gr);

	m_angle_per = std::make_shared<WidgetImageButton>(652, 270, "settings_units_per.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_angle_gr->clearTrigger();
				m_angle_selected = "%";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(719, 289, 225, 51, style, M("%")));
	addWidget(m_angle_per);

	m_energy_kkal = std::make_shared<WidgetImageButton>(967, 152, "settings_units_kkal.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_energy_j->clearTrigger();
				m_energy_kwh->clearTrigger();
				m_energy_selected = "ккал";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(1025, 150, 234, 51, style, M("ккал")));
	addWidget(m_energy_kkal);

	m_energy_j = std::make_shared<WidgetImageButton>(967, 231, "settings_units_j.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_energy_kkal->clearTrigger();
				m_energy_kwh->clearTrigger();
				m_energy_selected = "Дж";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(1025, 228, 234, 51, style, M("Дж")));
	addWidget(m_energy_j);

	m_energy_kwh = std::make_shared<WidgetImageButton>(967, 309, "settings_units_kwh.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_energy_kkal->clearTrigger();
				m_energy_j->clearTrigger();
				m_energy_selected = "кВт·ч";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(1025, 306, 234, 51, style, M("кВт·ч")));
	addWidget(m_energy_kwh);

	m_pwr_w = std::make_shared<WidgetImageButton>(337, 466, "settings_units_w.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_pwr_kw->clearTrigger();
				m_pwr_selected = "Вт";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(397, 481, 150, 51, style, M("Вт")));
	addWidget(m_pwr_w);

	m_pwr_kw = std::make_shared<WidgetImageButton>(337, 584, "settings_units_kw.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_pwr_w->clearTrigger();
				m_pwr_selected = "кВт";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(397, 598, 150, 51, style, M("кВт")));
	addWidget(m_pwr_kw);

	m_pwrair_met = std::make_shared<WidgetImageButton>(548, 466, "settings_units_met.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_pwrair_mmk->clearTrigger();
				m_pwrair_selected = "МЕТ";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(604, 481, 341, 51, style, M("МЕТ")));
	addWidget(m_pwrair_met);

	m_pwrair_mmk = std::make_shared<WidgetImageButton>(548, 584, "settings_units_mmk.png", "", "",
			[this](WidgetImageButton::Event e) {
				m_pwrair_met->clearTrigger();
				m_pwrair_selected = "мл/мин/кг";
				return nullptr;
		}, WidgetImageButton::Type::Trigger, std::make_shared<WidgetTextField>(604, 583, 341, 51, style, M("мл/мин/кг")));
	addWidget(m_pwrair_mmk);

	style.font_style = FontStyle::Normal;
	style.text_color_disabled = to_base(Colors::Black);
	addWidget(std::make_shared<WidgetTextField>(90, 181, 224, 51, style, M("километр")));
	addWidget(std::make_shared<WidgetTextField>(90, 259, 224, 51, style, M("метр")));
	addWidget(std::make_shared<WidgetTextField>(90, 337, 224, 51, style, M("миля")));
	addWidget(std::make_shared<WidgetTextField>(396, 181, 234, 51, style, M("километр в час")));
	addWidget(std::make_shared<WidgetTextField>(396, 259, 234, 51, style, M("метр в секунду")));
	addWidget(std::make_shared<WidgetTextField>(396, 337, 234, 51, style, M("миля в час")));
	addWidget(std::make_shared<WidgetTextField>(719, 201, 225, 51, style, M("градус")));
	addWidget(std::make_shared<WidgetTextField>(719, 320, 225, 51, style, M("процент")));
	addWidget(std::make_shared<WidgetTextField>(1025, 181, 234, 51, style, M("килокалория")));
	addWidget(std::make_shared<WidgetTextField>(1025, 259, 234, 51, style, M("джоуль")));
	addWidget(std::make_shared<WidgetTextField>(1025, 337, 234, 51, style, M("киловатт в час")));
	addWidget(std::make_shared<WidgetTextField>(397, 515, 150, 51, style, M("ватт")));
	addWidget(std::make_shared<WidgetTextField>(397, 632, 150, 51, style, M("киловатт")));
	addWidget(std::make_shared<WidgetTextField>(604, 515, 341, 51, style, M("метаболическая")));
	addWidget(std::make_shared<WidgetTextField>(604, 622, 341, 69, style, M("потребление кислорода\\на килограмм веса")));



	if (m_dist_selected == "км") {
		m_dist_km->setTrigger();
	} else if (m_dist_selected == "м") {
		m_dist_m->setTrigger();
	} else if (m_dist_selected == "мили") {
		m_dist_mile->setTrigger();
	}

	if (m_speed_selected == "км/ч") {
		m_speed_kmh->setTrigger();
	} else if (m_speed_selected == "м/с") {
		m_speed_ms->setTrigger();
	} else if (m_speed_selected == "миля/ч") {
		m_speed_mh->setTrigger();
	}

	if (m_angle_selected == "град") {
		m_angle_gr->setTrigger();
	} else if (m_angle_selected == "%") {
		m_angle_per->setTrigger();
	}

	if (m_energy_selected == "ккал") {
		m_energy_kkal->setTrigger();
	} else if (m_energy_selected == "Дж") {
		m_energy_j->setTrigger();
	} else if (m_energy_selected == "кВт·ч") {
		m_energy_kwh->setTrigger();
	}

	if (m_pwr_selected == "кВт") {
		m_pwr_kw->setTrigger();
	} else if (m_pwr_selected == "Вт") {
		m_pwr_w->setTrigger();
	}

	if (m_pwrair_selected == "МЕТ") {
		m_pwrair_met->setTrigger();
	} else if (m_pwrair_selected == "мл/мин/кг") {
		m_pwrair_mmk->setTrigger();
	}

}


SettingsUnitsMenu::~SettingsUnitsMenu() {
	LD("Dtor");

	if (m_dist_selected != ctx.m_db.getParam("ЕдиницыРасстояние")
			|| m_speed_selected != ctx.m_db.getParam("ЕдиницыСкорость")
			|| m_angle_selected != ctx.m_db.getParam("ЕдиницыУгол")
			|| m_energy_selected != ctx.m_db.getParam("ЕдиницыЭнергия")
			|| m_pwr_selected != ctx.m_db.getParam("ЕдиницыМощность")
			|| m_pwrair_selected != ctx.m_db.getParam("ЕдиницыМощностьАэробная"))
	{
		ctx.m_db.setParam("ЕдиницыРасстояние", m_dist_selected);
		ctx.m_db.setParam("ЕдиницыСкорость", m_speed_selected);
		ctx.m_db.setParam("ЕдиницыУгол", m_angle_selected);
		ctx.m_db.setParam("ЕдиницыЭнергия", m_energy_selected);
		ctx.m_db.setParam("ЕдиницыМощность", m_pwr_selected);
		ctx.m_db.setParam("ЕдиницыМощностьАэробная", m_pwrair_selected);
	}
}

