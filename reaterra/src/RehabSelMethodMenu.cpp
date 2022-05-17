#include "RehabSelMethodMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabExternalMenu.h"
#include "RehabMenu.h"
#include "RehabProtocolSelectionMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

RehabSelMethodMenu::RehabSelMethodMenu()
:	Menu {0, 0, "rehab_sel_method_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("выбор метода тестирования"), WIBFunc(), bc_last_style}
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

	addWidget(std::make_shared<WidgetImageButton>(222, 132, "rehab_sel_method_standard.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabProtocolSelectionMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(529, 133, 464, 170, style, M("ТЕСТИРОВАНИЕ\\ПО СТАНДАРТНЫМ\\ПРОТОКОЛАМ"))));
	addWidget(std::make_shared<WidgetImageButton>(222, 315, "rehab_sel_method_user.png", "", "",
			[](WidgetImageButton::Event e) {
				ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Protocol))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(529, 317, 464, 170, style, M("ТЕСТИРОВАНИЕ\\ПО ПОЛЬЗОВАТЕЛЬСКИМ\\ПРОТОКОЛАМ"))));
	addWidget(std::make_shared<WidgetImageButton>(222, 497, "rehab_sel_method_extern.png", "", "",
			[](WidgetImageButton::Event e) {
				return std::make_unique<MenuEvent>(std::make_unique<RehabExternalMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(529, 499, 464, 170, style, M("ТЕСТИРОВАНИЕ\\С ПРИМЕНЕНИЕМ\\ВНЕШНЕЙ СТРЕСС-СИСТЕМЫ"))));
}


RehabSelMethodMenu::~RehabSelMethodMenu() {
	LD("Dtor");
}

