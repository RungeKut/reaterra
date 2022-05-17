#include "RehabSelProfileMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
//#include "RehabProtocolSelectionMenu.h"
#include "RehabProfileRunMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

RehabSelProfileMenu::RehabSelProfileMenu()
:	Menu {0, 0, "rehab_external_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("выбор профиля"), WIBFunc(), bc_last_style}
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

	addWidget(std::make_shared<WidgetImageButton>(255, 199, "rehab_sel_profile_users_active.png", "", "rehab_sel_profile_users_passive.png",
			[](WidgetImageButton::Event e) {
				ctx.m_prev_menu["RehabUserProtocolsMenu.params"] = {std::to_string(to_base(RehabUserProtocolsMenu::ProMode::Profil))};
				return std::make_unique<MenuEvent>(std::make_unique<RehabUserProtocolsMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(451, 261, 516, 86, style, M("ПОЛЬЗОВАТЕЛЬСКИЕ\\БЕГОВЫЕ ПРОФИЛИ"))));
	WidgetImageButtonSP own = addWidget(std::make_shared<WidgetImageButton>(255, 407, "rehab_sel_profile_own_active.png", "rehab_sel_profile_own_disabled.png", "rehab_sel_profile_own_passive.png",
			[](WidgetImageButton::Event e) {
				ctx.m_prev_menu["RehabProfileRunMenu.params"] = {std::to_string(ctx.m_profile_id)};
				return std::make_unique<MenuEvent>(std::make_unique<RehabProfileRunMenu>());
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(451, 469, 516, 86, style, M("ПРОФИЛЬ ВЫБРАННОГО\\ПОЛЬЗОВАТЕЛЯ"))));

	if (ctx.m_profile_id == 0) {
		own->setState(Widget::State::Disabled);
	}
}

RehabSelProfileMenu::~RehabSelProfileMenu() {
	LD("Dtor");
}

