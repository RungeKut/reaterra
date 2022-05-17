#include "SettingsCalibrateMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "SettingsMenu.h"
#include "WidgetBreadCrumbs.h"

SettingsCalibrateMenu::SettingsCalibrateMenu()
:	Menu {0, 0, "settings_calibrate_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_angle {0}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("калибровка положения беговой дорожки"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	(m_negative = addWidget(std::make_shared<WidgetImageButton>(0, 99, "", "settings_calibrate_negative.png")))
			->setState(Widget::State::Disabled)->setVisible(false);
	(m_positive = addWidget(std::make_shared<WidgetImageButton>(0, 99, "", "settings_calibrate_positive.png")))
			->setState(Widget::State::Disabled)->setVisible(false);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.font_style = FontStyle::Bold;
	style.font_size = 25;
	style.text_color_disabled = 0x434242;
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(30, 638, 932, 88, style, M("Если калибровка по данной процедуре невозможна,\\"
			"необходимо отрегулировать ножки беговой дорожки так,\\чтобы угол подъема был наиболее близок к нулю.")));

	style.halign = Align::Center;
	style.update_func = [this](WidgetTextField &w) {
			char buff[10];
			sprintf(buff, " %4.2f ", m_angle / 100.);
			return M("УГОЛ ПОДЪЕМА") + buff + M("град");
		};
	m_angle_msg = addWidget(std::make_shared<WidgetTextField>(0, 46, 1280, 53, style));

	style.halign = Align::Left;
	style.update_func = [this](WidgetTextField &w) {
			std::string msg;
			m_negative->setVisible(false);
			m_positive->setVisible(false);
			if (m_angle < -DELTA_ANGLE) {
				msg = M("Отрегулируйте положение беговой дорожки,\\вращая ножки против часовой стрелки.");
				m_negative->setVisible(true);
			} else if (m_angle > DELTA_ANGLE) {
				msg = M("Отрегулируйте положение беговой дорожки,\\вращая ножки по часовой стрелке.");
				m_positive->setVisible(true);
			} else {
				msg = M("Регулировка положения беговой дорожки выполнена!");
			}
			return msg;
		};
	m_message = addWidget(std::make_shared<WidgetTextField>(30, 552, 832, 88, style));

	if (!ctx.m_treadmill.isCalibrating()) {
		ctx.m_treadmill.toggleMode();

		LD("Try entering to calibration mode");
		while (!ctx.m_treadmill.isCalibrating()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//TODO: добавить таймаут и обрабатывать выдачей ошибки
		}
	}

	m_timer.startTimerMillis(200);
}

SettingsCalibrateMenu::~SettingsCalibrateMenu() {
	if (ctx.m_treadmill.isCalibrating()) {
		ctx.m_treadmill.toggleMode();

		LD("Try quitting from calibration mode");
		while (ctx.m_treadmill.isCalibrating() || !ctx.m_treadmill.isReady()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//TODO: добавить таймаут и обрабатывать выдачей ошибки
		}
	}
	LD("Dtor");
}

Menu* SettingsCalibrateMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "run") {
		static int angle_prev = m_angle;
		m_angle = ctx.m_treadmill.getCalibrationAngleX();
		if (m_angle != angle_prev) {
			angle_prev = m_angle;
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	}

	ret = Menu::processEvent(std::move(event));
	return ret;
}
