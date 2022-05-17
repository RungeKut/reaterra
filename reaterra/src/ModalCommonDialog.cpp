#include "ModalCommonDialog.h"

#include "Context.h"
#include "WidgetImageButton.h"

//TODO: переделать диалоги на универсальный
ModalCommonDialog::ModalCommonDialog(std::string title, std::string message, std::string type, Flags flags, std::string icon_file, std::string button_text)
:	Menu {369, 262, "common_dialog_frame.png"},
	m_flags{flags}
{
	LD("Ctor " + type);
	m_check_events = false;
	setType(type);

	WTFStyle style {default_style};
	style.font_size = 22;
	style.font_style = FontStyle::Bold;
	if (!message.empty()) {
		int height = m_flags && Flags::Buttons ? 98 : 166;
		//TODO: использовать для цвета енумы стилей
		style.text_color_disabled = to_base(Colors::Black);
		(m_text = addWidget(std::make_shared<WidgetTextField>(16, 99, 511, height, style)))->setText(message);
	}


	if (m_flags && Flags::StopIcon) {
		addWidget(std::make_shared<WidgetImageButton>(231, 109, "", "icon_stop_passive.png", ""))
				->setState(Widget::State::Disabled);
	} else if (m_flags && Flags::FeetIcon) {
		addWidget(std::make_shared<WidgetImageButton>(231, 109, "", "icon_stay_straight.png", ""))
				->setState(Widget::State::Disabled);
	}

	int x = 16;
	int width = 511;
	std::shared_ptr<WidgetImageButton> icon;
	if (!icon_file.empty()) {
		icon = std::make_shared<WidgetImageButton>(16, 13, "", icon_file, icon_file,
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::IconClick));
					return nullptr;
				}, WidgetImageButton::Type::Clicker);
		x += icon->getWidth();
		width -= icon->getWidth();
		if (!(m_flags && Flags::IconClickable)) {
			icon->setState(Widget::State::Disabled);
		}
	}

	if (m_flags && Flags::GreenTitle) {
		addWidget(std::make_shared<WidgetImageButton>(16, 13, "", "common_dialog_green_title.png", "common_dialog_green_title.png"))
				->setState(Widget::State::Disabled);
		style.text_color_disabled = to_base(Colors::White);
		if (!(m_flags && Flags::NoSound)) {
			ctx.m_sound.play(SoundInterface::Sound::Info);
		}
	} else {
		addWidget(std::make_shared<WidgetImageButton>(16, 13, "", "common_dialog_yellow_title.png", "common_dialog_yellow_title.png"))
				->setState(Widget::State::Disabled);
		style.text_color_disabled = to_base(Colors::BlueOnYellow);
		if (!(m_flags && Flags::NoSound)) {
			ctx.m_sound.play(SoundInterface::Sound::Alarm);
		}
	}

	if (!title.empty()) {
		style.halign = Align::Left;
		addWidget(std::make_shared<WidgetTextField>(x, 13, width, 87, style))->setText(title);
	}

	if (icon) {
		addWidget(icon);
	}

	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	style.halign = Align::Center;
	if (flags && Flags::CloseButton) {
		addWidget(std::make_shared<WidgetImageButton>(184, 197, "modal_close_active.png", "", "button_close_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(193, 205, 122, 40, style, M("ЗАКРЫТЬ"))));
	} else if (flags && Flags::Cancel) {
		addWidget(std::make_shared<WidgetImageButton>(184, 197, "modal_close_active.png", "", "button_close_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(193, 205, 122, 40, style, M("ОТМЕНА"))));
	} else if (flags && Flags::YesNoCancel) {
		addWidget(std::make_shared<WidgetImageButton>(34, 197, "button_ok_short_active.png", "", "button_ok_short_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(42, 197, 64, 55, style, M("ДА"))));
		addWidget(std::make_shared<WidgetImageButton>(182, 197, "modal_close_active.png", "", "button_close_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(194, 197, 116, 55, style, M("ОТМЕНА"))));
		addWidget(std::make_shared<WidgetImageButton>(374, 197, "button_no_short_active.png", "", "button_no_short_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalNo));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(384, 197, 73, 55, style, M("НЕТ"))));
	} else if (flags && Flags::YesCancel) {
		addWidget(std::make_shared<WidgetImageButton>(110, 197, "button_ok_short_active.png", "", "button_ok_short_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(122, 197, 60, 55, style, M("ДА"))));
		addWidget(std::make_shared<WidgetImageButton>(259, 197, "modal_close_active.png", "", "button_close_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(266, 197, 123, 55, style, M("ОТМЕНА"))));
	} else if (flags && Flags::Yes) {
		addWidget(std::make_shared<WidgetImageButton>(201, 197, "modal_close_active.png", "", "button_close_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(216, 197, 100, 55, style, M("ДА"))));
	} else if (flags && Flags::EmptyButton) {
		addWidget(std::make_shared<WidgetImageButton>(184, 197, "button_empty_mid_active.png", "", "button_empty_mid_passive.png",
				[this](WidgetImageButton::Event e) {
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes));
					return nullptr;
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(193, 205, 158, 40, style, M(button_text))));

	}
}

ModalCommonDialog::~ModalCommonDialog() {
	LD("Dtor " + m_type);
}

void ModalCommonDialog::setTimer(int period, TimerFunc on_timer, int id, int count) {
	if (m_timer) {
		LE("Timer already set");
		throw std::runtime_error("ModalCommonDialog timer already set");
	}

	if (on_timer == nullptr && count > 0) {
		m_timer = std::make_unique<Timer>([this, count](int id) {
					if (++m_msg_counter >= count) {
						ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
					}
				}, id);
	} else {
		m_timer = std::make_unique<Timer>(on_timer, id);
	}
	m_timer->startTimerMillis(period);
}

void ModalCommonDialog::setText(std::string text) {
	if (m_text) {
		m_text->setText(text);
	}
}
