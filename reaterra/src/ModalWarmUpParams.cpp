#include "ModalWarmUpParams.h"

#include "Context.h"
#include "ModalKeyboardDigital.h"
#include "Utils.h"
#include "WidgetImageButton.h"

ModalWarmUpParams::ModalWarmUpParams()
: Menu {354, 130, "modal_big_frame_green.png"}
{
	LD("Ctor");
	setType("WUParam");

	addWidget(std::make_shared<WidgetImageButton>(25, 22, "icon_warm-up.png", "icon_warm-up.png", ""))->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(200, 326, "modal_close_active.png", "", "button_close_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(0) = m_begin_speed->getText();
				ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(1) = m_end_speed->getText();
				ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(2) = m_duration->getText();
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(200 + 9, 326 + 8, 122, 40, style, M("ЗАКРЫТЬ"))));


	style.halign = Align::Left;
	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(32, 116, 300, 49, style))->setText(M("НАЧАЛЬНАЯ СКОРОСТЬ") + ":");
	addWidget(std::make_shared<WidgetTextField>(32, 179, 300, 49, style))->setText(M("КОНЕЧНАЯ СКОРОСТЬ") + ":");
	addWidget(std::make_shared<WidgetTextField>(32, 241, 300, 49, style))->setText(M("ВРЕМЯ РАЗМИНКИ") + ":");
	addWidget(std::make_shared<WidgetTextField>(470, 116, 100, 49, style))->setText(M(ctx.m_db.getParam("ЕдиницыСкорость")));
	addWidget(std::make_shared<WidgetTextField>(470, 179, 100, 49, style))->setText(M(ctx.m_db.getParam("ЕдиницыСкорость")));
	addWidget(std::make_shared<WidgetTextField>(470, 241, 100, 49, style))->setText(M("мин"));

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(84, 15, 398, 88, style))->setText(M("ПАРАМЕТРЫ РАЗМИНКИ"));

	style = default_style;
	style.text_max_size = 10;
	style.halign = Align::Center;
	style.rect_color_passive = 0xECECEC;

	style.kbd_type = KbdType::Digital | KbdType::DotFlag;
	style.num_format = ctx.m_session.getSpeedFormat();
	style.min_limit = std::stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMinSpeed() / 10.));
	style.max_limit = std::stof(ctx.m_session.getSpeedString(ctx.m_treadmill.getMaxForwardSpeed() / 10.));
	m_begin_speed = addWidget(std::make_shared<WidgetTextField>(340, 116, 120, 49, style));
	m_begin_speed->setState(Widget::State::Passive);
	m_end_speed = addWidget(std::make_shared<WidgetTextField>(340, 179, 120, 49, style));
	m_end_speed->setState(Widget::State::Passive);
	style.kbd_type = KbdType::Digital;
	style.min_limit = 1;
	style.max_limit = 30;
	style.num_format = "";
	m_duration = addWidget(std::make_shared<WidgetTextField>(340, 241, 120, 49, style));
	m_duration->setState(Widget::State::Passive);

	m_begin_speed->setText(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(0));
	m_end_speed->setText(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(1));
	m_duration->setText(ctx.m_prev_menu["RehabBosBaseMenu.WarmUp"].at(2));
}

ModalWarmUpParams::~ModalWarmUpParams() {
	LD("Dtor");
}

