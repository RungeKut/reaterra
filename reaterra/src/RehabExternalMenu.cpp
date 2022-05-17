#include "RehabExternalMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
#include "RehabSelMethodMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetTextField.h"

RehabExternalMenu::RehabExternalMenu()
:	Menu {0, 0, "rehab_external_menu.png", {}, "REXT"}
{
	LD("Ctor");

	ctx.m_treadmill.stopInteraction();

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("тестирование с внешней стресс-системой"), WIBFunc(), bc_last_style}
	});

	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.font_style = FontStyle::Normal;
	style.font_size = 28;
	style.text_color_disabled = to_base(Colors::Black);
	addWidget(std::make_shared<WidgetTextField>(0, 47, 1280, 700, style))
		->setText(M("Управление осуществляется от внешней стресс-системы\\Для выхода необходимо нажать кнопку <НАЗАД>"));

	ctx.m_treadmill.setExternal(TreadmillDriver::ExtMode::On);
}

RehabExternalMenu::~RehabExternalMenu() {
	ctx.m_treadmill.setExternal(TreadmillDriver::ExtMode::Off);
	ctx.m_treadmill.startInteraction();

	LD("Dtor");
}

