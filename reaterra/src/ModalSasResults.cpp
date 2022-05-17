#include "ModalSasResults.h"

#include "Context.h"
#include "ModalKeyboardDigital.h"
#include "Utils.h"
#include "WidgetImageButton.h"

ModalSasResults::ModalSasResults()
: Menu {354, 130, "modal_big_frame_green.png"}
{
	LD("Ctor");
	setType("SasResults");

	addWidget(std::make_shared<WidgetImageButton>(16, 15, "icon_treadmill_green.png", "icon_treadmill_green.png", ""))->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.text_color_passive = 0x434242;
	style.text_color_active = to_base(Colors::White);
	style.font_size = 20;
	style.font_style = FontStyle::Bold;
	style.rect_color_active = to_base(Colors::None);
	addWidget(std::make_shared<WidgetImageButton>(200, 326, "modal_close_active.png", "", "button_close_passive.png",
			[this](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalCancel));
				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(200 + 9, 326 + 8, 122, 40, style, M("ЗАКРЫТЬ"))));


	style.halign = Align::Left;
	style.font_size = 20;
	style.font_style = FontStyle::Normal;
	style.text_color_disabled = 0x434242;
	addWidget(std::make_shared<WidgetTextField>(32, 110, 500, 49, style))->setText(M("ВРЕМЯ ПРОЦЕДУРЫ") + ": " + ctx.m_prev_menu["FreeRunMenuSas.RESULTS"].at(0));
	addWidget(std::make_shared<WidgetTextField>(32, 160, 500, 49, style))->setText(M("СИММЕТРИЧНОСТЬ ШАГА") + ": "
			+ M("Л-") + ctx.m_prev_menu["FreeRunMenuSas.RESULTS"].at(1) + "% / "+ M("П-") + ctx.m_prev_menu["FreeRunMenuSas.RESULTS"].at(2) + "%");
	addWidget(std::make_shared<WidgetTextField>(32, 210, 500, 49, style))->setText(M("СРЕДНЯЯ ДЛИНА ШАГА") + ": " + ctx.m_prev_menu["FreeRunMenuSas.RESULTS"].at(3));
	addWidget(std::make_shared<WidgetTextField>(32, 260, 500, 49, style))->setText(M("ПРОЙДЕННОЕ РАССТОЯНИЕ") + ": " + ctx.m_prev_menu["FreeRunMenuSas.RESULTS"].at(4));

	style.halign = Align::Center;
	style.font_style = FontStyle::Bold;
	style.font_size = 22;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(84, 15, 398, 88, style))->setText(M("РЕЗУЛЬТАТЫ ХОДЬБЫ"));

}

ModalSasResults::~ModalSasResults() {
	LD("Dtor");
}

