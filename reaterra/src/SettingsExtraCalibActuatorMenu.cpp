#include "SettingsExtraCalibActuatorMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "SettingsMenu.h"
#include "SettingsRegistersMenu.h"
#include "WidgetBreadCrumbs.h"

SettingsExtraCalibActuatorMenu::SettingsExtraCalibActuatorMenu()
:	Menu {0, 0, "settings_calibrate_menu.png"},
	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "run"));}, 0},
	m_angle {0}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("СИСТЕМА"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsRegistersMenu>());}, bc_link_style},
		{M("калибровка подъемника"), WIBFunc(), bc_last_style}
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
	style.font_style = FontStyle::Normal;
	style.font_size = 20;
	style.text_color_disabled = 0x434242;
	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(30, 538, 832, 188, style, M(
			"1. Необходимо отрегулировать полотно беговой дорожки\\"
			"паралельно земле по уровню.\\"
			"2. Нажать кнопку кнопку 'СБРОС'\\"
			"(для сброса предыдущей калибровки)\\"
			"3. Нажать кнопку 'ОТКАЛИБРОВАТЬ'\\"
			"4. После этого угол наклона должен быть от -0,1 до 0,1\\"
			"5. Выйти из меню кнопкой 'НАЗАД', проверить установку\\"
			"угла в режиме свободный бег. ")));

	style.halign = Align::Center;
	style.font_size = 25;
	style.update_func = [this](WidgetTextField &w) {
			char buff[10];
			sprintf(buff, " %4.2f ", m_angle / 100.);
			m_negative->setVisible(false);
			m_positive->setVisible(false);
			if (m_angle < -DELTA_ANGLE) {
				m_negative->setVisible(true);
			} else if (m_angle > DELTA_ANGLE) {
				m_positive->setVisible(true);
			}
			return M("УГОЛ ПОДЪЕМА") + buff + M("град");
		};
	m_angle_msg = addWidget(std::make_shared<WidgetTextField>(0, 46, 1280, 53, style));

	WTFStyle style_button = default_style;
	style_button.font_size = 22;
	style_button.text_color_active = to_base(Colors::BlueOnYellow);
	style_button.text_color_passive = to_base(Colors::White);
	style_button.rect_color_active = to_base(Colors::None);
	style_button.text = M("СБРОС");
	addWidget(std::make_shared<WidgetImageButton>(964, 118, "settings_reg_active.png",
			"settings_reg_disabled.png", "settings_reg_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_treadmill.actuatorCalibReset();
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("КАЛИБРОВКА"),
						M("СБРОС КАЛИБРОВКИ ВЫПОЛНЕН"), "CROK", Flags::GreenTitle | Flags::CloseButton,
						"icon_treadmill_green.png");
				return std::unique_ptr<MenuEvent>();
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(964, 118, 270, 62, style_button)));

	style_button.text = M("КАЛИБРОВКА");
	addWidget(std::make_shared<WidgetImageButton>(964, 191, "settings_reg_active.png",
			"settings_reg_disabled.png", "settings_reg_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_treadmill.actuatorCalibSet();
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::make_unique<ModalCommonDialog>(M("КАЛИБРОВКА"),
						M("КАЛИБРОВКА ПОДЪЕМНИКА\\ВЫПОЛНЕНА"), "CCOK", Flags::GreenTitle | Flags::CloseButton,
						"icon_treadmill_green.png");
				return std::unique_ptr<MenuEvent>();
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(964, 191, 270, 62, style_button)));

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


SettingsExtraCalibActuatorMenu::~SettingsExtraCalibActuatorMenu() {
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

Menu* SettingsExtraCalibActuatorMenu::processEvent(std::unique_ptr<Event> event) {
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
