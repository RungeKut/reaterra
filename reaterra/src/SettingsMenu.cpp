#include "SettingsMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsAccelMenu.h"
#include "SettingsCalibrateMenu.h"
#include "SettingsClockMenu.h"
#include "SettingsExportMenu.h"
#include "SettingsBosParamsMenu.h"
#include "SettingsPasswordMenu.h"
#include "SettingsRegistersMenu.h"
#include "SettingsUnitsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"

SettingsMenu::SettingsMenu(std::string user)
:	Menu {0, 0, "settings_main.png"}
{
	LD("Ctor");
	setType("SETM");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("настройки"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Right;
	style.lines = 6;
	style.hpad = 0;
	style.font_size = 22;
	style.rect_color_active = to_base(Colors::None);;

	addWidget(std::make_shared<WidgetImageButton>(39, 111, "settings_main_accel_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsAccelMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(40, 261, 271, 121, style, "\\" + M("ЗАДАНИЕ\\УСКОРЕНИЯ И\\ТОРМОЖЕНИЯ"))
	));

	addWidget(std::make_shared<WidgetImageButton>(651, 111, "settings_main_calib_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsCalibrateMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(652, 261, 271, 121, style, "\\" + M("КАЛИБРОВКА\\ПОЛОЖЕНИЯ\\БЕГОВОЙ ДОРОЖКИ"))
	));

	addWidget(std::make_shared<WidgetImageButton>(39, 408, "settings_main_dtime_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsClockMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(40, 558, 271, 121, style, "\\\\" + M("НАСТРОЙКА\\ВРЕМЕНИ И ДАТЫ"))
	));

	addWidget(std::make_shared<WidgetImageButton>(958, 111, "settings_main_hrate_active.png", "", "",
			[this](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsBosParamsMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(959, 261, 271, 121, style, "\\" + M("НАСТРОЙКА\\ПАРАМЕТРОВ\\РЕЖИМОВ С БОС"))
	));

	addWidget(std::make_shared<WidgetImageButton>(345, 111, "settings_main_import_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsExportMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(346, 261, 271, 121, style, "\\\\" + M("ИМПОРТ И\\ЭКСПОРТ ДАННЫХ"))
	));

	addWidget(std::make_shared<WidgetImageButton>(651, 408, "settings_main_names_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsUnitsMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(652, 558, 271, 121, style, M("ПЕРЕКЛЮЧЕНИЕ\\ЕДИНИЦ ИЗМЕРЕНИЯ\\ФИЗИЧЕСКИХ\\ВЕЛИЧИН"))
	));

	addWidget(std::make_shared<WidgetImageButton>(345, 408, "settings_main_pwd_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsPasswordMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(346, 558, 271, 121, style, "\\\\\\" + M("РЕДАКТОР ПАРОЛЕЙ"))
	));

	auto w = addWidget(std::make_shared<WidgetImageButton>(958, 408, "settings_main_service_active.png", "settings_main_service_disabled.png", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<SettingsRegistersMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(959, 558, 271, 121, style, "\\\\" + M("СЕРВИСНАЯ\\ИНФОРМАЦИЯ"))
	));

	if (ctx.m_admin != "admin") {
		w->setState(Widget::State::Disabled);
	}
}

SettingsMenu::~SettingsMenu() {
	LD("Dtor");
}

