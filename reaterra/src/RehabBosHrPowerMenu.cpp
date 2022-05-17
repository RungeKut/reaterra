#include "RehabBosHrPowerMenu.h"

#include "Context.h"
#include "ModalCommonDialog.h"


RehabBosHrPowerMenu::RehabBosHrPowerMenu()
:	RehabBosBaseMenu {Mode::Power}
{
	LD("Ctor");

	addWidget(std::make_shared<WidgetTextField>(5, 440, 407, 56, m_label_style, M("Номинальная мощность")));
	addWidget(std::make_shared<WidgetTextField>(524, 440, 154, 56, m_unit_style, M(ctx.m_db.getParam("ЕдиницыМощность")))); // Единица измерения параметра "Номинальная мощность"
	int btn_offset = 100;
	m_angle_label->setX(m_angle_label->getX() - btn_offset);
	m_speed_label->setX(m_speed_label->getX() + btn_offset);
	m_angle_btn->setX(m_angle_btn->getX() - btn_offset);
	m_speed_btn->setX(m_speed_btn->getX() + btn_offset);
	m_speed_angle_label->setX(m_speed_angle_label->getX() + btn_offset);
	m_speed_angle_label->setVisible(false);
	m_speed_angle_btn->setState(Widget::State::Disabled);
	m_speed_angle_btn->setVisible(false);
	m_field_style.kbd_type = KbdType::Digital | KbdType::DotFlag;
	m_field_style.num_format = ctx.m_session.getPowerMechFormat();
	m_field_style.check_func = CheckFunc();
	m_power_text = addWidget(std::make_shared<WidgetTextField>(397, 440, 127, 56, m_field_style, ctx.m_session.getPowerMechString(0))); // Поле ввода параметра "Номинальная мощность"
	m_power_text->setState(Widget::State::Passive);
	onFieldChange("", 0);


	if (ctx.m_patient.empty()) {
		using Flags = ModalCommonDialog::Flags;
		m_modal = std::make_unique<ModalCommonDialog>(M("НЕ УСТАНОВЛЕН ПАЦИЕНТ"),
				M("УПРАВЛЕНИЕ ПО МОЩНОСТИ\\ОТКЛЮЧЕНО"), "HRENOPAT", Flags::YellowTitle | Flags::CloseButton,
				"icon_treadmill_exclamation_yellow.png");
	}

	update();
}


RehabBosHrPowerMenu::~RehabBosHrPowerMenu() {
	LD("Dtor");
}


void RehabBosHrPowerMenu::onStateChange(Widget::State state) {
	m_power_text->setState(state);
}


int RehabBosHrPowerMenu::getBosCorrectionDelta() {
	int ret = 0;

	float power = ctx.m_session.getPowerWatt(stof(m_power_text->getText()));
	float power_min = power * (1 - 0.05);
	float power_max = power * (1 + 0.05);

	float cur_power = ctx.m_session.getPowerMech();

	if (cur_power < power_min) {
		ret = cur_power - power_min;
	} else if (cur_power > power_max) {
		ret = cur_power - power_max;
	}

	LD("BosCorrectionDelta: " + std::to_string(ret));
	return ret;
}


RehabBosBaseMenu::BosCorrectionData RehabBosHrPowerMenu::getBosCorrectionData(int delta) {
	BosCorrectionData ret;
	if (delta < 0) {
		delta = abs(delta);
		if (delta < 5) {
			ret = BosCorrectionData {10, 1, 10};
		} else if (delta >= 5 && delta < 15) {
			ret = BosCorrectionData {10, 2, 10};
		} else if (delta >= 15 && delta < 30) {
			ret = BosCorrectionData {10, 3, 10};
		} else if (delta >= 30 && delta < 50) {
			ret = BosCorrectionData {10, 4, 10};
		} else {
			ret = BosCorrectionData {10, 5, 10};
		}
	} else if (delta > 0) {
		if (delta < 5) {
			ret = BosCorrectionData {12, 1, 10};
		} else if (delta >= 5 && delta < 15) {
			ret = BosCorrectionData {12, 1, 10};
		} else if (delta >= 15 && delta < 30) {
			ret = BosCorrectionData {12, 1, 10};
		} else if (delta >= 30 && delta < 50) {
			ret = BosCorrectionData {12, 1, 10};
		} else {
			ret = BosCorrectionData {12, 1, 10};
		}
	} else {
		ret = {10, 0, 0};
	}

	LD("BOS correction data delta:" + std::to_string(delta) + "- speed: " + std::to_string(ret.speed) + "   angle: " + std::to_string(ret.angle) + "    time: " + std::to_string(ret.interval));
	return ret;
}

void RehabBosHrPowerMenu::onFieldChange(std::string field, float value) {
	float max_speed = stof(m_max_speed_text->getText());
	float max_angle = stof(m_max_angle_text->getText());

	if (field == "max_speed") {
		max_speed = value;
	} else if (field == "max_angle") {
		max_angle = value;
	}

	m_power_text->setLimits(stof(ctx.m_session.getPowerMechString(ctx.m_session.getPowerMechByParams(ctx.m_treadmill.getMinSpeed() / 10., 0, ctx.m_patient_weight))),
			stof(ctx.m_session.getPowerMechString(ctx.m_session.getPowerMechByParams(ctx.m_session.getSpeedKmH(max_speed),
			ctx.m_session.getAngleDegree(max_angle), ctx.m_patient_weight))));
}
