#include "SettingsHrMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "SettingsHrMenu.h"
#include "WidgetBreadCrumbs.h"


SettingsHrMenu::SettingsHrMenu()
:	Menu {0, 0, "settings_hr_menu.png"},
	m_timer {[this](int id) {
			if (++m_search_cnt < 11) {
				m_search_ind->setDisabled("settings_hr_search" + std::to_string(m_search_cnt) + ".png");
				updateTable();
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
			} else {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer));
			}
		}, 0},
	m_hrate {"- - -"},
	m_battery {"- - -"},
	m_search_cnt {0},
	m_selected_row {0}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("подключение/отключение датчика ЧСС"), WIBFunc(), bc_last_style}
	});


	addWidget(std::make_shared<WidgetText>(1025, 357, "- - -", Align::Center, 45, 0xFFFFFF, 0xFFFFFF,
				[this]() {
					int hr = static_cast<int>(ctx.m_hr_monitor.getHRate());
					return hr ? std::to_string(hr) : "- - -";
				}));
	addWidget(std::make_shared<WidgetText>(1131, 480, "- - -", Align::Center, 45, 0xFFFFFF, 0xFFFFFF,
				[this]() {
					int bat = static_cast<int>(ctx.m_hr_monitor.getBattery());
					return bat < 0 ? "- - -" : std::to_string(bat);
				}));

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	m_search_ind = std::make_shared<WidgetImageButton>(848, 228, "", "");
	addWidget(m_search_ind);
	m_search_ind->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = 0x434242;
	style.halign = Align::Center;
	style.lines = 6;
	style.hpad = 0;
	style.font_size = 28;
	style.rect_color_active = to_base(Colors::None);;

	addWidget(std::make_shared<WidgetTextField>(0, 46, 794, 56, style, M("НАЙДЕННЫЕ BLUETOOTH УСТРОЙСТВА")));

	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(824, 295, 161, 94, style, M("ПУЛЬС")));
	addWidget(std::make_shared<WidgetTextField>(824, 389, 268, 148, style, M("УРОВЕНЬ\\ЗАРЯДА\\ДАТЧИКА ЧСС")));
	style.halign = Align::Right;
	addWidget(std::make_shared<WidgetTextField>(1066, 295, 183, 94, style, M("удар/мин")));
	addWidget(std::make_shared<WidgetTextField>(1171, 389, 78, 148, style, M("%")));

	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(0, 102, 333, 51, style, M("SSID")));
	addWidget(std::make_shared<WidgetTextField>(333, 102, 461, 51, style, M("НАЗВАНИЕ")));


	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	m_search = std::make_shared<WidgetImageButton>(832, 74, "settings_hr_search.png", "", "",
			[this](WidgetImageButton::Event e) {
				ctx.m_hr_monitor.clearStoredDevice();
				if (m_search_cnt) {
					stopSearch();
				} else {
					m_search_cnt = 1;
					m_selected_row = 0;
					m_search_ind->setDisabled("settings_hr_search" + std::to_string(m_search_cnt) + ".png");
					m_search->setPassive("settings_hr_search.png");
					if (ctx.m_hr_monitor.isConnected()) {
						ctx.m_hr_monitor.disconnect();
					}
					m_disconnect->setState(Widget::State::Disabled);
					m_timer.startTimerMillis(2000);
					for (unsigned i = 0; i < TABLE_ROWS; ++i) {
						m_list.at(i)->setState(Widget::State::Disabled);
						m_text_list.at(i).at(0)->setText("");
						m_text_list.at(i).at(1)->setText("");
						m_list.at(i)->clearTrigger();
					}
					show(true);
					ctx.m_hr_monitor.startDiscovery();
				}
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(833, 75, 291, 124, style, M("ПОИСК\\БЕСПРОВОДНОГО\\ДАТЧИКА ЧСС")));
	addWidget(m_search);

	m_disconnect = std::make_shared<WidgetImageButton>(832, 578, "settings_hr_disconnect_active.png", "settings_hr_disconnect_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				ctx.m_hr_monitor.clearStoredDevice();
				if (ctx.m_hr_monitor.isConnected()) {
					if (!m_modal) {
						using Flags = ModalCommonDialog::Flags;
						m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОТКЛЮЧЕНИЕ ДАТЧИКА ЧСС"),
								M("ПОЖАЛУЙСТА, ПОДОЖДИТЕ"), "HRDC", Flags::GreenTitle | Flags::NoSound,
								"icon_hr_cross_green.png"));
					}
					show(true);
					ctx.m_hr_monitor.disconnect();
				}
				m_selected_row = 0;
				m_disconnect->setState(Widget::State::Disabled);
				updateTable();
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(833, 579, 291, 124, style, M("ОТКЛЮЧЕНИЕ\\БЕСПРОВОДНОГО\\ДАТЧИКА ЧСС")));
	addWidget(m_disconnect);
	if (ctx.m_db.getParam("СенсорЧСС").empty()) {
		m_disconnect->setState(Widget::State::Disabled);
	}

	int cols[] = {0, 333, 795};

	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		m_list.push_back(std::make_shared<WidgetImageButton>(0, ROW_START_Y + i * ROW_STEP_Y, "settings_hr_select" + std::to_string(i + 1) + ".png", "", "",
				[this, i](WidgetImageButton::Event e) {
					ctx.m_hr_monitor.clearStoredDevice();
					for (unsigned k = 0; k < m_list.size(); ++k) {
						if (k != i) {
							m_list.at(k)->clearTrigger();
						}
					}
					m_selected_row = i + 1;
					if (!m_modal) {
						using Flags = ModalCommonDialog::Flags;
						m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ПОДКЛЮЧЕНИЕ ДАТЧИКА ЧСС"),
								M("ПОЖАЛУЙСТА, ПОДОЖДИТЕ"), "HRCN", Flags::GreenTitle | Flags::NoSound,
								"icon_hr_ok_green.png"));
					}
					show(true);
					LD("Selected: " + std::to_string(m_selected_row));
					stopSearch();
					ctx.m_hr_monitor.disconnect();
					ctx.m_hr_monitor.connect(ctx.m_hr_monitor.getDeviceAt(m_selected_row - 1)->path, false);
					return nullptr;
			}, WidgetImageButton::Type::Trigger));
		addWidget(m_list.back());

		m_text_list.push_back({});
		m_text_list.back().push_back(std::make_shared<WidgetText>(cols[0] + (cols[1] - cols[0]) / 2, ROW_START_Y + (i + 0.5) * ROW_STEP_Y, "",
				Align::Center, 25, 0x0, 0x0, std::function<std::string(void)>{}, FontStyle::Normal, (cols[1] - cols[0]), 2));
		addWidget(m_text_list.back().at(0));
		m_text_list.back().push_back(std::make_shared<WidgetText>(cols[1] + (cols[2] - cols[1]) / 2, ROW_START_Y + (i + 0.5) * ROW_STEP_Y, "",
				Align::Center, 25, 0x0, 0x0, std::function<std::string(void)>{}, FontStyle::Normal, (cols[2] - cols[1]), 2));
		addWidget(m_text_list.back().at(1));
	}

	updateTable();
}

SettingsHrMenu::~SettingsHrMenu() {
	LD("Dtor");
}

Menu* SettingsHrMenu::processEvent(std::unique_ptr<Event> event) {
	LD("processEvent " + event->getName() + "   extra: " + event->getExtra());
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer) {
		stopSearch();
	}

	if (m_disconnect->getState() == Widget::State::Disabled && ctx.m_hr_monitor.isConnected()) {
		m_disconnect->setState(Widget::State::Passive);
	}

	if (m_modal && m_modal->getType().substr(0, 2) == "HR" && event->getType() == Event::EventType::ModalYes) {
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}


	if ((!m_modal || m_modal->getType() == "HRCN") && event->getType() == Event::EventType::HrMonitorEvent && event->getExtra() == "BAD DEVICE") {
		ctx.m_hr_monitor.disconnect();
		m_selected_row = 0;
		m_disconnect->setState(Widget::State::Disabled);
		updateTable();
		Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА! ПОДКЛЮЧЕНИЕ К\\НЕИЗВЕСТНОМУ УСТРОЙСТВУ"),
				M("ВЫБЕРИТЕ УСТРОЙСТВО «POLAR»\\ИЛИ СОВМЕСТИМОЕ С НИМ"), "HRER", Flags::YellowTitle | Flags::CloseButton,
				"icon_hr_exclamation_yellow.png"));
	}


	LD("before if " + event->getName() + "   extra: " + event->getExtra() + "  modal:" + (m_modal?"true":"false"));
	if (/*m_modal && m_modal->getType() == "HRCN" && */event->getType() == Event::EventType::HrMonitorEvent && event->getExtra() == "CONNECTED") {
		m_modal = nullptr;
		updateTable();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	if (m_modal && m_modal->getType() == "HRCN" && event->getType() == Event::EventType::HrMonitorEvent && event->getExtra() == "CONNECTION_ERROR") {
		LD("CONNECTION_ERROR");
		m_modal.reset();
		m_selected_row = 0;
		updateTable();
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА ПОДКЛЮЧЕНИЯ\\К ДАТЧИКУ ЧСС"),
				M("ПОВТОРИТЕ ПОИСК"), "HRER", Flags::YellowTitle | Flags::CloseButton,
				"icon_hr_exclamation_yellow.png"));
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	if (m_modal && m_modal->getType() == "HRDC" && event->getType() == Event::EventType::HrMonitorEvent && event->getExtra() == "DISCONNECTED") {
		m_modal = nullptr;
	}

	ret = Menu::processEvent(std::move(event));
	return ret;
}

void SettingsHrMenu::updateTable() {
	LD("CLEAR");
	for (unsigned i = 0; i < TABLE_ROWS; ++i) {
		if (i < ctx.m_hr_monitor.getDeviceListSize()) {
			m_list.at(i)->setState(Widget::State::Passive);
			m_text_list.at(i).at(0)->setText(ctx.m_hr_monitor.getDeviceAt(i)->address);
			m_text_list.at(i).at(1)->setText(ctx.m_hr_monitor.getDeviceAt(i)->name);
			if (ctx.m_hr_monitor.getDeviceAt(i)->connected || ctx.m_hr_monitor.getDeviceAt(i)->path == ctx.m_hr_monitor.m_monitor_path) {
				m_selected_row = i + 1;
				LD("row is selected: " + std::to_string(m_selected_row));
			}
		} else {
			m_list.at(i)->setState(Widget::State::Disabled);
			m_text_list.at(i).at(0)->setText("");
			m_text_list.at(i).at(1)->setText("");
		}
		m_list.at(i)->clearTrigger();

	}

	if (m_selected_row) {
		m_list.at(m_selected_row - 1)->setTrigger();
	}
}

void SettingsHrMenu::stopSearch() {
	m_search_ind->setDisabled("");
	m_search->setPassive("");
	m_timer.stopTimer();
	show(true);
	if (m_search_cnt) {
		ctx.m_hr_monitor.stopDiscovery();
	}
	m_search_cnt = 0;
	ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
}
