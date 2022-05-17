#include "RehabMenu.h"

#include "Context.h"
#include "FreeRunMenu.h"
#include "FreeRunMenuSas.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ProceduresMenu.h"
#include "RehabSelHrMenu.h"
#include "RehabSelMethodMenu.h"
#include "RehabSelProfileMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

RehabMenu::RehabMenu()
:	Menu {0, 0, "rehab_procedures_main_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("выбор типа процедур"), WIBFunc(), bc_last_style}
	});

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x787878;
	style.halign = Align::Right;
	style.lines = 6;
	style.font_size = 26;
	style.rect_color_active = to_base(Colors::None);;

	addWidget(std::make_shared<WidgetImageButton>(646, 135, "rehab_procedures_main_menu-freerun_activ.png", "", "",
			[](WidgetImageButton::Event e) {
				return (ctx.m_settings.getParamInt("sas_enable") != 0 || ctx.m_treadmill.isSasExists())
						? std::make_unique<MenuEvent>(std::make_unique<FreeRunMenuSas>())
						: std::make_unique<MenuEvent>(std::make_unique<FreeRunMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(647, 214, 474, 145, style, "\\\\\\\\" + M("СВОБОДНЫЙ БЕГ"))));
	addWidget(std::make_shared<WidgetImageButton>(646, 402, "rehab_procedures_main_menu-hrate_active.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabSelHrMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(647, 481, 474, 145, style, "\\\\" + M("БЕГ\\С БИОЛОГИЧЕСКОЙ\\ОБРАТНОЙ СВЯЗЬЮ ПО ЧСС"))));
	addWidget(std::make_shared<WidgetImageButton>(118, 402, "rehab_procedures_main_menu-profile_activ.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabSelProfileMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(119, 481, 474, 145, style, "\\\\\\" + M("БЕГ\\ПО ПРОФИЛЮ"))));
	addWidget(std::make_shared<WidgetImageButton>(118, 135, "rehab_procedures_main_menu-test_activ.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabSelMethodMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(119, 214, 474, 145, style, "\\\\\\\\" + M("ТЕСТИРОВАНИЕ"))));
}

RehabMenu::~RehabMenu() {
	LD("Dtor");
}
