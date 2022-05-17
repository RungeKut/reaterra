#include "ModalPatientHistoryFilter.h"

#include "Context.h"
#include "ModalKeyboardAlpha.h"
#include "Utils.h"
#include "WidgetImageButton.h"

ModalPatientHistoryFilter::ModalPatientHistoryFilter()
: Menu {354, 130, "patient_history_filter_menu.png"}
{
	LD("Ctor");
	setType("PFLT");
	WTFStyle style {default_style};

	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(180, 326, "modal_close_active.png", "", "button_close_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(180 + 9, 326 + 8, 122, 40, style, M("ЗАКРЫТЬ"))));

	addWidget(std::make_shared<WidgetImageButton>(29, 326, "button_ok_short_active.png", "", "button_ok_short_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				ctx.m_prev_menu["PatientHistoryFilters"] = {
								"PatientHistoryFilters",
								m_from_entry->getText(),
								m_to_entry->getText(),
								m_family_entry->getText()
						};
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(29 + 9, 326 + 8, 60, 40, style, M("ОК"))));

	addWidget(std::make_shared<WidgetImageButton>(369, 326, "button_no_mid_active.png", "", "button_no_mid_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				ctx.m_prev_menu["PatientHistoryFilters"] = {
								"PatientHistoryFilters",
								"",
								"",
								""
						};
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(369 + 9, 326 + 8, 122, 40, style, M("СБРОС"))));


	style.halign = Align::Left;
	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(52, 116, 259, 49, style))->setText(M("НАЧАЛЬНАЯ ДАТА") + ":");
	addWidget(std::make_shared<WidgetTextField>(52, 179, 259, 49, style))->setText(M("КОНЕЧНАЯ ДАТА") + ":");
	addWidget(std::make_shared<WidgetTextField>(52, 241, 259, 49, style))->setText(M("ФАМИЛИЯ") + ":");

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(84, 15, 398, 88, style))->setText(M("УСТАНОВИТЕ ФИЛЬТР\\ОТБОРА ДАННЫХ"));

	style = default_style;
	style.text_max_size = 10;
	style.halign = Align::Left;

	style.kbd_type = KbdType::Date;
	m_from_entry = addWidget(std::make_shared<WidgetTextField>(292, 116, 200, 49, style));
	m_from_entry->setState(Widget::State::Passive);
	m_to_entry = addWidget(std::make_shared<WidgetTextField>(292, 179, 200, 49, style));
	m_to_entry->setState(Widget::State::Passive);
	style.kbd_type = KbdType::Alpha;
	m_family_entry = addWidget(std::make_shared<WidgetTextField>(221, 241, 271, 49, style));
	m_family_entry->setState(Widget::State::Passive);

	m_from_entry->setText(ctx.m_prev_menu["PatientHistoryFilters"].at(1));
	m_to_entry->setText(ctx.m_prev_menu["PatientHistoryFilters"].at(2));
	m_family_entry->setText(ctx.m_prev_menu["PatientHistoryFilters"].at(3));
}

ModalPatientHistoryFilter::~ModalPatientHistoryFilter() {
	LD("Dtor");
}

