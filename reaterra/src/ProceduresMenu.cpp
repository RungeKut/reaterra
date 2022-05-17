#include "ProceduresMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

ProceduresMenu::ProceduresMenu()
:	Menu {0, 0, "procedures_main_menu.png", {
		std::make_shared<WidgetImageButton>(271, 196, "procedures_main_menu-rehab_activ.png", "", "",
				[](WidgetImageButton::Event e) {
					return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());
				}),
		std::make_shared<WidgetImageButton>(647, 196, "procedures_main_menu-sportfit_activ.png"),
		std::make_shared<WidgetImageButton>(0, 746, "button_back_active.png", "", "",
				[](WidgetImageButton::Event e) {
					return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());
				})
	}}
{
	LD("Ctor");
}

ProceduresMenu::~ProceduresMenu() {
	LD("Dtor");
}

