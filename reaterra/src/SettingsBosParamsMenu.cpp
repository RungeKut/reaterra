#include "SettingsBosParamsMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"

SettingsBosParamsMenu::SettingsBosParamsMenu()
:	Menu {0, 0, "settings_bos_params_menu.png"}
{
	LD("Ctor");

	std::array<std::tuple<std::string, float, float>, 3> format_low = {{
			{ctx.m_session.getSpeedFormat(), stof(ctx.m_session.getSpeedString(0.1)), stof(ctx.m_session.getSpeedString(1))},
			{ctx.m_session.getAngleFormat(), stof(ctx.m_session.getAngleString(0.5)), stof(ctx.m_session.getAngleString(2))},
			{"%3.0f", 10, 60}
	}};
	std::array<std::tuple<std::string, float, float>, 3> format_high = {{
			{ctx.m_session.getSpeedFormat(), stof(ctx.m_session.getSpeedString(0.1)), stof(ctx.m_session.getSpeedString(3))},
			{ctx.m_session.getAngleFormat(), stof(ctx.m_session.getAngleString(0.5)), stof(ctx.m_session.getAngleString(3))},
			{"%3.0f", 5, 30}
	}};


	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>());}, bc_link_style},
		{M("настройка параметров режимов с БОС"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_size = 22;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = to_base(Colors::Black);
	style.halign = Align::Left;
	style.lines = 4;
	addWidget(std::make_shared<WidgetTextField>(195, 50, 892, 42, style, M("Изменение нагрузки при достижении нижнего уровня ЧСС")));
	addWidget(std::make_shared<WidgetTextField>(195, 400, 892, 42, style, M("Изменение нагрузки при запрограммированном верхнем уровне ЧСС")));
	style.halign = Align::Center;
	style.font_size = 16;
	style.font_style = FontStyle::Bold;
	addWidget(std::make_shared<WidgetTextField>(195, 92, 294, 86, style, M("Разница между измеренной\\(текущей) и установленной\\(минимальной) ЧСС,\\удар/мин")));
	addWidget(std::make_shared<WidgetTextField>(489, 92, 199, 86, style, M("Скорость") +  ",\\" + M(ctx.m_db.getParam("ЕдиницыСкорость"))));
	addWidget(std::make_shared<WidgetTextField>(688, 92, 199, 86, style, M("Угол подьема") + ",\\" + M(ctx.m_db.getParam("ЕдиницыУгол"))));
	addWidget(std::make_shared<WidgetTextField>(886, 92, 199, 86, style, M("Время,\\сек")));
	addWidget(std::make_shared<WidgetTextField>(195, 443, 294, 86, style, M("Разница между измеренной\\(текущей) и установленной\\(минимальной) ЧСС,\\удар/мин")));
	addWidget(std::make_shared<WidgetTextField>(489, 443, 199, 86, style, M("Скорость") +  ",\\" + M(ctx.m_db.getParam("ЕдиницыСкорость"))));
	addWidget(std::make_shared<WidgetTextField>(688, 443, 199, 86, style, M("Угол подьема") + ",\\" + M(ctx.m_db.getParam("ЕдиницыУгол"))));
	addWidget(std::make_shared<WidgetTextField>(886, 443, 199, 86, style, M("Время,\\сек")));
	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	addWidget(std::make_shared<WidgetTextField>(195, 178, 294, 42, style, M("0 – 5,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 220, 294, 42, style, M("5,1 – 15,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 263, 294, 42, style, M("15,1 – 30,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 306, 294, 42, style, M("30,1 – 50,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 350, 294, 42, style, M("> 50,1")));
	addWidget(std::make_shared<WidgetTextField>(195, 178 + 351, 294, 42, style, M("0 – 5,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 220 + 351, 294, 42, style, M("5,1 – 15,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 263 + 351, 294, 42, style, M("15,1 – 30,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 306 + 351, 294, 42, style, M("30,1 – 50,0")));
	addWidget(std::make_shared<WidgetTextField>(195, 350 + 351, 294, 42, style, M("> 50,1")));

	std::array<int, 4> cols = {488, 687, 886, 886 + 199};
	int ROW_STEP_Y = 43;
	style.kbd_type = KbdType::Digital | KbdType::DotFlag;

	int ROW_START_Y = 178;
	style.check_func = [this, format_low](std::string s, WidgetTextField &w) {
		char buff[100];
		sprintf(buff, std::get<0>(format_low[w.getIdCol()]).c_str(), std::stof(s));
		std::string val = buff;
		if (w.getIdCol() == 0) { // Скорость
			val = std::to_string(ctx.m_session.getSpeedKmH(std::stof(val)));;
		} else if (w.getIdCol() == 1) { // Угол
			val = std::to_string(ctx.m_session.getAngleDegree(std::stof(val)));
		}
		m_bos_params_low.at(3 * w.getIdRow() + w.getIdCol()) = val;
		w.setText(buff);
		ctx.m_db.setParam("ПараметрыБocНижний", Utils::unsplit(m_bos_params_low, Context::PARAMS_SEPARATOR));
		return "";
	};

	m_bos_params_low = getParamsLow();
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 3; ++j) {
			std::string val = m_bos_params_low.at(3 * i + j);
			if (j == 0) { // Скорость
				val = ctx.m_session.getSpeedString(stof(val));
			} else if (j == 1) { // Угол
				val = ctx.m_session.getAngleString(stof(val));
			}
			addWidget(std::make_shared<WidgetTextField>(cols[j] + 1, ROW_START_Y + i * ROW_STEP_Y,
					cols[j + 1] - cols[j] - 1, ROW_STEP_Y - 1, style))->setIdCol(j).setIdRow(i).setText(val).
							setLimits(std::get<1>(format_low[j]), std::get<2>(format_low[j])).setState(Widget::State::Passive);
		}
	}


	ROW_START_Y = 178 + 350;
	style.check_func = [this, format_high](std::string s, WidgetTextField &w) {
		char buff[100];
		sprintf(buff, std::get<0>(format_high[w.getIdCol()]).c_str(), std::stof(s));
		std::string val = buff;
		if (w.getIdCol() == 0) { // Скорость
			val = std::to_string(ctx.m_session.getSpeedKmH(std::stof(val)));;
		} else if (w.getIdCol() == 1) { // Угол
			val = std::to_string(ctx.m_session.getAngleDegree(std::stof(val)));
		}
		m_bos_params_high.at(3 * w.getIdRow() + w.getIdCol()) = val;
		w.setText(buff);
		ctx.m_db.setParam("ПараметрыБocВерхний", Utils::unsplit(m_bos_params_high, Context::PARAMS_SEPARATOR));
		return "";
	};

	m_bos_params_high = getParamsHigh();
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 3; ++j) {
			std::string val = m_bos_params_high.at(3 * i + j);
			if (j == 0) { // Скорость
				val = ctx.m_session.getSpeedString(stof(val));
			} else if (j == 1) { // Угол
				val = ctx.m_session.getAngleString(stof(val));
			}
			addWidget(std::make_shared<WidgetTextField>(cols[j] + 1, ROW_START_Y + i * ROW_STEP_Y,
					cols[j + 1] - cols[j] - 1, ROW_STEP_Y - 1, style))->setIdCol(j).setIdRow(i).setText(val).
							setLimits(std::get<1>(format_high[j]), std::get<2>(format_high[j])).setState(Widget::State::Passive);
		}
	}

}

const std::vector<std::string> SettingsBosParamsMenu::getDefaultLow() {
	return 	{
			"0.1", "0.5", "25",  // Скорость, Угол, Время
			"0.2", "0.5", "25",
			"0.3", "0.5", "25",
			"0.5", "1.0", "25",
			"0.5", "1.0", "25"
	};
}

const std::vector<std::string> SettingsBosParamsMenu::getDefaultHigh() {
	return 	{
			"0.5", "0.5", "12",
			"0.8", "1.0", "12",
			"1.0", "1.0", "10",
			"1.2", "1.2", "8",
			"2.0", "1.6", "7",
	};
}

std::vector<std::string> SettingsBosParamsMenu::getParamsLow() {
	return Utils::split(ctx.m_db.getParam("ПараметрыБocНижний", Utils::unsplit(getDefaultLow(), Context::PARAMS_SEPARATOR)), Context::PARAMS_SEPARATOR);
}

std::vector<std::string> SettingsBosParamsMenu::getParamsHigh() {
	return Utils::split(ctx.m_db.getParam("ПараметрыБocВерхний", Utils::unsplit(getDefaultHigh(), Context::PARAMS_SEPARATOR)), Context::PARAMS_SEPARATOR);
}

float SettingsBosParamsMenu::getSpeed(std::vector<std::string> data, int row) {
	return stof(data.at(3 * row));
}

float SettingsBosParamsMenu::getAngle(std::vector<std::string> data, int row) {
	return stof(data.at(3 * row + 1));
}

float SettingsBosParamsMenu::getTime(std::vector<std::string> data, int row) {
	return stof(data.at(3 * row + 2));
}

SettingsBosParamsMenu::~SettingsBosParamsMenu() {
	LD("Dtor");
}

