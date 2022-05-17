#include "RehabProtocolSelectionMenu.h"

#include "MainMenu.h"
#include "MenuEvent.h"
#include "RehabMenu.h"
#include "RehabTestProtocolMenu.h"
#include "RehabSelMethodMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "WidgetText.h"

RehabProtocolSelectionMenu::RehabProtocolSelectionMenu()
:	Menu {0, 0, "rehab_sel_protocol_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("ПРОЦЕДУРЫ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<RehabMenu>());}, bc_link_style},
		{M("выбор протокола тестирования"), WIBFunc(), bc_last_style}
	});

	int off = 47;
	int y[] = {68, 151, 234, 317, 400, 483, 566, 649};

	m_protocols = ctx.m_db.getProtocols("ПРОТ10");

	for (unsigned i = 0; i < m_protocols.size(); ++i) {
		addWidget(std::make_shared<WidgetImageButton>(387, y[i], "rehab_sel_protocol_" + std::to_string(i + 1) +".png", "", "",
				[this, i](WidgetImageButton::Event e) {
					ctx.m_prev_menu["RehabTestProtocolMenu.params"] = {m_protocols.at(i).at(static_cast<int>(DbConnector::ProtocolList::Id))};
					return std::make_unique<MenuEvent>(std::make_unique<RehabTestProtocolMenu>());
				}));
		addWidget(std::make_shared<WidgetText>(639, y[i] + off, M("Протокол ")
				+ m_protocols.at(i).at(static_cast<int>(DbConnector::ProtocolList::Name)), Align::Center, 30, 0xFFFFFF, 0xFFFFFF));
	}
}

RehabProtocolSelectionMenu::~RehabProtocolSelectionMenu() {
	LD("Dtor");
}
