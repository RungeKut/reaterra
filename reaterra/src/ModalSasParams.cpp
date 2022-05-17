#include "ModalSasParams.h"

#include "Context.h"
#include "ModalKeyboardDigital.h"
#include "Utils.h"
#include "WidgetImageButton.h"

ModalSasParams::ModalSasParams()
: Menu {354, 190, "sas_freerun_rehab_procedures_main_menu_hrsettings_window.png"},
  m_alarm_flag {ctx.m_prev_menu["FreeRunMenuSas.HR"].at(2)}
{
	LD("Ctor");
	setType("HRP");

	addWidget(std::make_shared<WidgetImageButton>(29, 32, "", "sas_freerun_rehab_procedures_main_menu_hrsettings_icon.png", ""))->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(326, 262, "modal_close_active.png", "", "sas_modal_close_hrsettings_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(337, 280, 117, 23, style, M("ОТМЕНА"))));
	addWidget(std::make_shared<WidgetImageButton>(65, 262, "sas_modal_save_active.png", "", "sas_modal_save_hrsettings_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_prev_menu["FreeRunMenuSas.HR"].at(0) = m_hr_max->getText();
				ctx.m_prev_menu["FreeRunMenuSas.HR"].at(1) = m_hr_crit->getText();
				ctx.m_prev_menu["FreeRunMenuSas.HR"].at(2) = m_alarm_flag;
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(81, 280, 150, 23, style, M("СОХРАНИТЬ"))));


	style.halign = Align::Right;
	style.font_size = 21;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(15, 131, 263, 23, style))->setText(M("Максимальная ЧСС") + ":");
	addWidget(std::make_shared<WidgetTextField>(15, 192, 263, 23, style))->setText(M("Критическая ЧСС") + ":");

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 23;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(102, 49, 423, 23, style))->setText(M("ВЕРХНИЕ ГРАНИЦЫ ЧСС"));

	style = default_style;
	style.text_max_size = 10;
	style.halign = Align::Center;
	style.rect_color_passive = 0xECECEC;

	style.kbd_type = KbdType::Digital;
	style.min_limit = 40;
	style.max_limit = std::stoi(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(1)) - 1;
	m_hr_max = addWidget(std::make_shared<WidgetTextField>(289, 118, 167, 49, style));
	m_hr_max->setState(Widget::State::Passive);
	style.min_limit = 50;
	style.max_limit = 300;
	style.check_func = [this](std::string s, WidgetTextField &w) {
			int max_limit = std::stoi(s) - 1;
			m_hr_max->setMaxLimit(max_limit);
			if (std::stoi(m_hr_max->getText()) > max_limit) {
				m_hr_max->setText(std::to_string(max_limit));
			}
			return "";
		};
	m_hr_crit = addWidget(std::make_shared<WidgetTextField>(289, 181, 167, 49, style));
	m_hr_crit->setState(Widget::State::Passive);

	std::function<void(bool)> alarm_update = [this](bool change) {
		if (change) {
			m_alarm_flag = (m_alarm_flag == "0" ? "1" : "0");
		}

		m_hr_alarm->setPassive(m_alarm_flag == "0"
				? "sas_freerun_rehab_procedures_hr_alarm_disable.png"
				:"sas_freerun_rehab_procedures_hr_alarm_passive.png");
	};

	style.check_func = CheckFunc();
	m_hr_alarm = addWidget(std::make_shared<WidgetImageButton>(471, 118,
				"sas_freerun_rehab_procedures_hr_alarm_active.png",
				"sas_freerun_rehab_procedures_hr_alarm_disable.png",
				"sas_freerun_rehab_procedures_hr_alarm_passive.png",
				[this, alarm_update](WidgetImageButton::Event e) {
					alarm_update(true);
					return nullptr;
				}, WidgetImageButton::Type::Clicker));
	m_hr_max->setText(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(0));
	m_hr_crit->setText(ctx.m_prev_menu["FreeRunMenuSas.HR"].at(1));
	m_alarm_flag = ctx.m_prev_menu["FreeRunMenuSas.HR"].at(2);
	alarm_update(false);
}

ModalSasParams::~ModalSasParams() {
	LD("Dtor");
}

