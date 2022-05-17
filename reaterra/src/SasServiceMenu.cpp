#include "SasServiceMenu.h"

#include "Context.h"

SasServiceMenu::SasServiceMenu()
:	Menu {0, 1, "test_white.png", {}, "SSSR"}
{
	LD("Ctor");
	ctx.m_treadmill.m_read_sas_service_regs = true;
	WTFStyle style {default_style};

	style.kbd_type = KbdType::None;
	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 18;
	style.text_color_disabled = 0xFF0000;
	addWidget(std::make_shared<WidgetTextField>(10, 20, 1260, 24, style, M("СЕРВИСНОЕ МЕНЮ")));
	m_message = addWidget(std::make_shared<WidgetTextField>(10, 70, 1260, 24, style, M("ДИНАМИЧЕСКИЙ ТЕСТ")));

	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.rect_color_active = to_base(Colors::None);
	m_stat_btn = addWidget(std::make_shared<WidgetImageButton>(100, 680, "button_empty_mid_active.png", "", "button_empty_mid_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_treadmill.startSasTest(true);
				m_message->setText("СТАТИЧЕСКИЙ ТЕСТ");
				m_stat_btn->setVisible(false);
				m_stat_btn->setState(Widget::State::Disabled);
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(110, 695, 158, 24, style, M("СТАТ.ТЕСТ"))));

	style.rect_color_active = 0xFF0000;
	style.rect_color_passive = 0xFF0000;
	style.text_color_disabled = 0x434242;
	style.text_color_passive = 0xFFFF00;
	style.text_color_active = style.text_color_passive;
	for (int i = 0; i < 4; ++i) {
		style.font_size = 22;
		addWidget(std::make_shared<WidgetTextField>(100 + 1260 / 4 * i, 150, 150, 24, style, M("Плата") + " " + std::to_string(i + 1)));
		for (int j = 0; j < 16; ++j) {
			if (i == 0 && j == 8) {
				break;
			}

			style.font_size = 18;
			m_sensors.push_back(addWidget(std::make_shared<WidgetTextField>(100 + 1260 / 4 * i, 185 + j * 35, 150, 24, style, M("Сенсор") + " " + std::to_string(m_sensors.size() + 1))));
		}
	}

	updateSensors();

}

SasServiceMenu::~SasServiceMenu() {
	LD("Dtor");
}

Menu* SasServiceMenu::processEvent(std::unique_ptr<Event> event) {
	if (event->getType() == Event::EventType::Refresh) {
		updateSensors();
	}
	if (event->getType() == Event::EventType::Touch){
		touchProcess(std::move(event));
	}

	return nullptr;
}

void SasServiceMenu::updateSensors() {
	std::vector<unsigned short> sens = ctx.m_treadmill.getSasSensorsStatus();

	LD("UPDATE: " + std::to_string(sens.at(0)) + " " + std::to_string(sens.at(1)) + " " + std::to_string(sens.at(2)) + " " + std::to_string(sens.at(3)));

	int counter = 0;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 16; ++j) {
			if (i == 0 && j == 8) {
				break;
			}

			bool state = (sens.at(i) >> j) & 1;
			m_sensors.at(counter++)->setState(state ? Widget::State::Passive : Widget::State::Disabled);
		}
	}

}
