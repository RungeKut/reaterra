#include "RehabSelHrMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabBosHrMinMaxMenu.h"
#include "RehabBosHrPowerMenu.h"
#include "RehabMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"

RehabSelHrMenu::RehabSelHrMenu()
:	Menu {0, 0, "rehab_procedures_hr_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("выбор процедуры с БОС"), WIBFunc(), bc_last_style}
	});

	WTFStyle style {default_style};
	style.text_color_disabled = 0x333333;
	style.lines = 2;
	style.kbd_type = KbdType::None;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.halign = Align::Right;
	style.font_style = FontStyle::Normal;
	style.rect_color_active = to_base(Colors::None);;
	style.font_size = 26;

	addWidget(std::make_shared<WidgetImageButton>(254, 198, "rehab_procedures_hr_sel_minmax_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabBosHrMinMaxMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(555, 199, 420, 193, style, M("ЗАДАНИЕ ПО\\ЧСС МИН / ЧСС МАКС"))));
	addWidget(std::make_shared<WidgetImageButton>(254, 406, "rehab_procedures_hr_sel_power_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabBosHrPowerMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(555, 407, 420, 193, style, M("ЗАДАНИЕ ПО\\ЧСС / МОЩНОСТИ"))));
}

RehabSelHrMenu::~RehabSelHrMenu() {
	LD("Dtor");
}

