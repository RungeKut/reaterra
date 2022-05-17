#include "RehabBosHrMinMaxMenu.h"

#include "Context.h"
#include "SettingsBosParamsMenu.h"

RehabBosHrMinMaxMenu::RehabBosHrMinMaxMenu()
:	RehabBosBaseMenu {Mode::MinMax}
{
	LD("Ctor");

	m_bell = true;
	m_hr_alarm_cnt = 0;
	m_extra_line->setVisible(true);
	addWidget(std::make_shared<WidgetTextField>(5, 440, 407, 56, m_label_style, M("Максимальное значение ЧСС")));
	m_unit_style.halign = Align::Left;
	addWidget(std::make_shared<WidgetTextField>(524, 440, 154, 56, m_unit_style, M("уд/мин"))); // Единица измерения параметра "Минимальное значение ЧСС"
	m_unit_style.halign = Align::Center;

	m_field_style.min_limit = 40;
	m_field_style.max_limit = std::stoi(m_max_possible_hr_text->getText());
	m_field_style.check_func = [this](std::string s, WidgetTextField &w) {
			int max_limit = std::stoi(s);
			m_min_hr_text->setMaxLimit(max_limit - 1);
			if (std::stoi(m_min_hr_text->getText()) >= max_limit) {
				m_min_hr_text->setText(std::to_string(max_limit - 1));
			}
			return "";
		};
	m_max_hr_text = addWidget(std::make_shared<WidgetTextField>(397, 440, 127, 56, m_field_style, "105")); // Поле ввода параметра "Максимальное значение ЧСС"
	m_max_hr_text->setState(Widget::State::Passive);

	m_proc_time_label->setY(497 + 57);
	m_proc_time_unit_label->setY(497 + 57);
	m_proc_time_text->setY(497 + 57);
	addWidget(std::make_shared<WidgetTextField>(5, 497, 407, 56, m_label_style, M("Минимальное значение ЧСС")));
	addWidget(std::make_shared<WidgetTextField>(524, 497, 154, 56, m_unit_style, M("уд/мин"))); // Единица измерения параметра "Минимальное значение ЧСС"
	m_field_style.num_format = "%3.0f";
	m_field_style.check_func = CheckFunc();
	m_field_style.min_limit = 30;
	m_field_style.max_limit = std::stoi(m_max_hr_text->getText());
	m_min_hr_text = addWidget(std::make_shared<WidgetTextField>(397, 497, 127, 56, m_field_style, "30"));
	m_min_hr_text->setState(Widget::State::Passive); // Поле ввода параметра "Минимальное значение ЧСС"
	m_load_control->setY(598);
	m_angle_btn->setY(m_angle_btn->getY() + 2);
	m_speed_btn->setY(m_speed_btn->getY() + 2);
	m_speed_angle_btn->setX(263);
	m_speed_angle_btn->setY(640);
	m_speed_angle_btn->setPassive("rehab_procedures_hr_speed_angle_small_passive.png");
	m_speed_angle_btn->setActive("rehab_procedures_hr_speed_angle_small_active.png");
	m_speed_angle_btn->setDisabled("rehab_procedures_hr_speed_angle_small_disabled.png");

	std::string check = m_bell ? "rehab_procedures_hr_bell_on.png" : "rehab_procedures_hr_bell_off.png";
	m_bell_btn = addWidget(std::make_shared<WidgetImageButton>(630, 443, "rehab_procedures_hr_bell_active.png", "", check,
			[this](WidgetImageButton::Event e) {
				m_bell = !m_bell;
				if (m_bell) {
					m_bell_btn->setPassive("rehab_procedures_hr_bell_on.png");
				} else {
					m_bell_btn->setPassive("rehab_procedures_hr_bell_off.png");
				}
				return nullptr;
		}));

	m_bos_params_low = SettingsBosParamsMenu::getParamsLow();
	m_bos_params_high = SettingsBosParamsMenu::getParamsHigh();

	update();
}


RehabBosHrMinMaxMenu::~RehabBosHrMinMaxMenu() {
	LD("Dtor");
}


void RehabBosHrMinMaxMenu::onMaxPossibleHrChange(int value) {
	m_max_hr_text->setMaxLimit(value);
	if (std::stoi(m_max_hr_text->getText()) > value) {
		m_max_hr_text->setText(std::to_string(value));
	}
	if (std::stoi(m_min_hr_text->getText()) >= value) {
		m_min_hr_text->setText(std::to_string(value - 1));
	}
}


void RehabBosHrMinMaxMenu::onStateChange(Widget::State state) {
	m_max_hr_text->setState(state);
	m_min_hr_text->setState(state);
	m_proc_time_text->setState(state);
}


int RehabBosHrMinMaxMenu::getBosCorrectionDelta() {
	int ret = 0;
	int hrate = ctx.m_session.getHrate();

	if (hrate < std::stoi(m_min_hr_text->getText())) {
		ret = hrate - std::stoi(m_min_hr_text->getText());
	} else if (hrate > std::stoi(m_max_hr_text->getText())) {
		ret =hrate - std::stoi(m_max_hr_text->getText());
	}

	return ret;
}


RehabBosBaseMenu::BosCorrectionData RehabBosHrMinMaxMenu::getBosCorrectionData(int delta) {
	BosCorrectionData ret;
	int row;
	if (delta < 0) {
		delta = abs(delta);
		if (delta < 5) {
			row = 0;
		} else if (delta >= 5 && delta < 15) {
			row = 1;
		} else if (delta >= 15 && delta < 30) {
			row = 2;
		} else if (delta >= 30 && delta < 50) {
			row = 3;
		} else {
			row = 4;
		}
		ret = BosCorrectionData {
			static_cast<int>(SettingsBosParamsMenu::getTime(m_bos_params_low, row)),
			static_cast<int>(SettingsBosParamsMenu::getSpeed(m_bos_params_low, row) * 10),
			static_cast<int>(SettingsBosParamsMenu::getAngle(m_bos_params_low, row) * 100),
		};
	} else if (delta > 0) {
		if (delta < 5) {
			row = 0;
		} else if (delta >= 5 && delta < 15) {
			row = 1;
		} else if (delta >= 15 && delta < 30) {
			row = 2;
		} else if (delta >= 30 && delta < 50) {
			row = 3;
		} else {
			row = 4;
		}
		ret = BosCorrectionData {
			static_cast<int>(SettingsBosParamsMenu::getTime(m_bos_params_high, row)),
			static_cast<int>(SettingsBosParamsMenu::getSpeed(m_bos_params_high, row) * 10),
			static_cast<int>(SettingsBosParamsMenu::getAngle(m_bos_params_high, row) * 100),
		};
	} else {
		ret = {10, 0, 0};
	}
	LD("BOS correction data delta:" + std::to_string(delta) + "- speed: " + std::to_string(ret.speed) + "   angle: " + std::to_string(ret.angle) + "    time: " + std::to_string(ret.interval));
	return ret;
}


void RehabBosHrMinMaxMenu::onTick() {
	if (ctx.m_treadmill.getState() == TreadmillDriver::RunState::Run
			&& m_bell
			&& ctx.m_session.getHrate() > 0
			&& ctx.m_session.getHrate() > stoi(m_max_hr_text->getText()))
	{
		if (++m_hr_alarm_cnt > 3) {
			m_hr_alarm_cnt = 0;
			ctx.m_sound.play(SoundInterface::Sound::Bell);
		}
	} else {
		m_hr_alarm_cnt = 0;
	}

}
