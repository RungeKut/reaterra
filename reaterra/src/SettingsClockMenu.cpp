#include "SettingsClockMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"
#include "WidgetBreadCrumbs.h"
#include "WidgetImageButton.h"
#include "Utils.h"

const unsigned SettingsClockMenu::DAYS[] = {30, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 , 31};

SettingsClockMenu::SettingsClockMenu()
:	Menu {0, 0, "settings_clock_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_counter {0}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("настройка времени и даты"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	style.font_size = 28;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	addWidget(std::make_shared<WidgetTextField>(250, 211, 259, 62, style, M("ЧАСЫ")));
	addWidget(std::make_shared<WidgetTextField>(511, 211, 259, 62, style, M("МИНУТЫ")));
	addWidget(std::make_shared<WidgetTextField>(770, 211, 259, 62, style, M("СЕКУНДЫ")));
	addWidget(std::make_shared<WidgetTextField>(250, 421, 259, 62, style, M("ЧИСЛО")));
	addWidget(std::make_shared<WidgetTextField>(511, 421, 259, 62, style, M("МЕСЯЦ")));
	addWidget(std::make_shared<WidgetTextField>(770, 421, 259, 62, style, M("ГОД")));

	style.font_style = FontStyle::Bold;
	style.font_size = 25;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(0, 46, 1280, 53, style, M("НАСТРОЙКА ВРЕМЕНИ И ДАТЫ")));

	m_current_time = *Utils::getCurrentTime();
	style.text_max_size = 2;

	style.font_size = 48;
	style.rect_color_active = 0xACD5E6;
	style.kbd_type = KbdType::Digital;
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_passive = to_base(Colors::BlueOnYellow);
	style.update_func = [this](WidgetTextField &w) {
			if (w.getState() != Widget::State::Active) {
				char buff[5] = "";
				switch (to_enum<Field>(w.getIdCol())) {
				case Field::Seconds:
					sprintf(buff, "%02d", m_current_time.tm_sec);
					break;
				case Field::Minutes:
					sprintf(buff, "%02d", m_current_time.tm_min);
					break;
				case Field::Hours:
					sprintf(buff, "%02d", m_current_time.tm_hour);
					break;
				case Field::Days:
					sprintf(buff, "%02d", m_current_time.tm_mday);
					break;
				case Field::Months:
					sprintf(buff, "%02d", m_current_time.tm_mon + 1);
					break;
				case Field::Years:
					sprintf(buff, "%04d", m_current_time.tm_year + 1900);
					break;
				}
				return std::string{buff};
			} else {
				return w.getText();
			}
		};

	style.check_func = [this](std::string s, WidgetTextField &w) {
		int v = std::stoi(s);
		switch (to_enum<Field>(w.getIdCol())) {
		case Field::Seconds:
			m_current_time.tm_sec = v;
			break;
		case Field::Minutes:
			m_current_time.tm_min = v;
			break;
		case Field::Hours:
			m_current_time.tm_hour = v;
			break;
		case Field::Days:
			m_current_time.tm_mday = v;
			break;
		case Field::Months:
			m_current_time.tm_mon = v - 1;
			daysCorrection(m_current_time.tm_year + 1900);
			break;
		case Field::Years:
			m_current_time.tm_year = v - 1900;
			daysCorrection(m_current_time.tm_year + 1900);
			break;
		}
		if (int ret = Utils::setDateTime(&m_current_time)) {
			LE("Error datetime setting - " + std::to_string(ret));
		}

		return "";
	};

	addWidget(std::make_shared<WidgetTextField>(250, 272, 259, 101, style))
			->setLimits(0,23).setIdCol(to_base(Field::Hours)).setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(511, 272, 259, 101, style))
			->setLimits(0,59).setIdCol(to_base(Field::Minutes)).setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(770, 272, 259, 101, style))
			->setLimits(0,59).setIdCol(to_base(Field::Seconds)).setState(Widget::State::Passive);
	m_days = std::make_shared<WidgetTextField>(250, 483, 259, 101, style);
	addWidget(m_days)->setLimits(1,31).setIdCol(to_base(Field::Days)).setState(Widget::State::Passive);
	addWidget(std::make_shared<WidgetTextField>(511, 483, 259, 101, style))
			->setLimits(1,12).setIdCol(to_base(Field::Months)).setState(Widget::State::Passive);
	style.text_max_size = 4;
	addWidget(std::make_shared<WidgetTextField>(770, 483, 259, 101, style))
			->setLimits(2018,2099).setIdCol(to_base(Field::Years)).setState(Widget::State::Passive);

	daysCorrection(m_current_time.tm_year + 1900);

	m_timer.startTimerMillis(100);
}

SettingsClockMenu::~SettingsClockMenu() {
	LD("Dtor");
}

Menu* SettingsClockMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "run") {
		if (++m_counter >= 10) {
			m_counter = 0;
			if(++m_current_time.tm_sec > 59) {
				m_current_time = *Utils::getCurrentTime();
			}
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	}

	ret = Menu::processEvent(std::move(event));
	return ret;
}

void SettingsClockMenu::daysCorrection(unsigned year) {
	if (m_current_time.tm_mon == 1) {
		int feb_days = Utils::isLeapYear(year) ? 29 : 28;
		m_days->setLimits(1, feb_days);
		if (m_current_time.tm_mday > feb_days) {
			m_current_time.tm_mday = feb_days;
		}
	} else {
		m_days->setLimits(1, DAYS[m_current_time.tm_mon]);
	}
}
