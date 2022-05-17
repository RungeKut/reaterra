#include "SettingsExtraCalibDriveMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "SettingsMenu.h"
#include "SettingsRegistersMenu.h"
#include "WidgetBreadCrumbs.h"

SettingsExtraCalibDriveMenu::SettingsExtraCalibDriveMenu()
:	Menu {0, 0, "settings_registers_menu.png"}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("СИСТЕМА"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsRegistersMenu>());}, bc_link_style},
		{M("калибровка привода"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.rect_color_active = to_base(Colors::None);
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::White);
	style.font_style = FontStyle::Normal;
	style.font_size = 20;
	style.text_color_disabled = 0x434242;
	style.halign = Align::Center;
	addWidget(std::make_shared<WidgetTextField>(0, 60, 1280, 154, style, M(
			"Данное меню предназначено исключительно для разработчиков!\\"
			"Если вы по каким-то причинам видите данное меню, значит произошел сбой! НЕ ИЗМЕНЯЙТЕ ПАРАМЕТРЫ В ОКНЕ!\\"
			"Выключите и включите Беговую дорожку! Если меню возникнет снова обратитесь в сервисную службу.\\"
			"Тел. +7 (831) 461-87-86, +7 (831) 461-88-86")));

	style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(30, 210, 1280, 320, style, M(
			"1. Нажмите кнопку 'ЗАПУСК'\\"
			"2. Произведите замер скорости бегового полотна при помощи\\"
			"тахометра цифрового комбинированного\\"
			"3. Нажмите кнопку 'СТОП'\\"
			"4. Запишите полученное значение в окно 'Текущая скорость м/мин'\\"
			"5. Нажмите кнопку 'Калибровать'\\"
			"6. Перейдите в меню 'СВОБОДНЫЙ БЕГ' и запустите дорожку на максимальной скорости.\\"
			"7. Произведите замер скорости бегового полотна при помощи\\"
			"тахометра цифрового комбинированного, скорость полотна должна быть:\\"
			"от 165 до 168 м/мин для модификации 10 км/ч\\"
			"от 365 до 368 м/мин для модификации 22 км/ч\\"
			"от 491 до 508 м/мин для модификации 30 км/ч")));

	style.font_size = 25;
	addWidget(std::make_shared<WidgetTextField>(384, 585, 400, 54, style, M("Текущая скорость м/мин:")));
	addWidget(std::make_shared<WidgetTextField>(384, 657, 400, 54, style, M("Полученный коэффициент:")));

	style.rect_color_disabled = 0xEBECEC;
	WidgetTextFieldSP calib_coef = addWidget(std::make_shared<WidgetTextField>(750, 657, 150, 54, style, "- - -"));

	WTFStyle style_button = default_style;
	style_button.font_size = 22;
	style_button.text_color_active = to_base(Colors::BlueOnYellow);
	style_button.text_color_passive = to_base(Colors::White);
	style_button.rect_color_active = to_base(Colors::None);
	style_button.text = M("КАЛИБРОВКА");
	WidgetImageButtonSP calib_button = addWidget(std::make_shared<WidgetImageButton>(70, 652, "settings_reg_active.png",
			"settings_reg_disabled.png", "settings_reg_passive.png",
			[this, calib_coef](WidgetImageButton::Event e) {
				unsigned short coef = std::stoul(calib_coef->getText());
				ctx.m_treadmill.setSpeedCalib(coef);
				using Flags = ModalCommonDialog::Flags;
				//TODO: имена иконок в енумы
				m_modal = std::make_unique<ModalCommonDialog>(M("КАЛИБРОВКА"),
						M("КАЛИБРОВКА ПРИВОДА ВЫПОЛНЕНА"), "CDOK", Flags::GreenTitle | Flags::CloseButton,
						"icon_treadmill_green.png");
				return std::unique_ptr<MenuEvent>();
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(70, 652, 270, 62, style_button)));
	calib_button->setState(Widget::State::Disabled);

	style.kbd_type = KbdType::Digital | KbdType::DotFlag;
	style.rect_color_active = 0x87CEEA;
	style.rect_color_passive = 0xCBCCCC;
	style.text_color_passive = to_base(Colors::Black);
	style.text_color_active = to_base(Colors::Black);
	style.num_format = "%5.1f";
	style.text_max_size = 5;
	style.check_func = [this, calib_coef, calib_button](std::string s, WidgetTextField &w) {
		float speed = std::stof(s);
		if (speed != 0) {
			unsigned short coef = ctx.m_treadmill.getMaxForwardSpeed() * 50 * 100 / (speed / 100 * 60);
			calib_coef->setText(std::to_string(coef));
			if (coef > 3500 && coef < 7500) {
				calib_button->setState(Widget::State::Passive);
			} else {
				calib_button->setState(Widget::State::Disabled);
			}
		} else {
			calib_coef->setText("- - -");
			calib_button->setState(Widget::State::Disabled);
		}
		return "";
	};

	WidgetTextFieldSP calib_speed = std::make_shared<WidgetTextField>(750, 585, 150, 54, style, "0.0");
	addWidget(calib_speed)
		->setLimits(0, 1000);

	style_button.text = M("ЗАПУСК");
	WidgetTextFieldSP start_text = std::make_shared<WidgetTextField>(70, 580, 270, 62, style_button);
	addWidget(std::make_shared<WidgetImageButton>(70, 580, "settings_reg_active.png", "", "settings_reg_passive.png",
			[start_text, calib_button, calib_coef, calib_speed](WidgetImageButton::Event e) {
				static bool running = false;
				running = !running;

				if (running) {
					start_text->setText(M("СТОП"));
					calib_button->setState(Widget::State::Disabled);
					calib_speed->setText("0.0").setState(Widget::State::Disabled);
					calib_coef->setText("- - -");
				} else {
					start_text->setText(M("ЗАПУСК"));
					calib_speed->setState(Widget::State::Passive);
				}

				ctx.m_treadmill.setDNF(running);
				return std::unique_ptr<MenuEvent>();
			}, WidgetImageButton::Type::Clicker, start_text));
}

SettingsExtraCalibDriveMenu::~SettingsExtraCalibDriveMenu() {
	ctx.m_treadmill.setDNF(false);
	LD("Dtor");
}

//TODO: удалить не нужные processEvents - там где только закрытие модальных окон
