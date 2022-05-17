#include "ModalListSelector.h"

#include "Context.h"
#include "WidgetText.h"

ModalListSelector::ModalListSelector(int x, int y, DataSet options, std::string active_pic, std::string passive_pic, uint32_t color_text_active, uint32_t color_text, int font_size)
:	Menu {x, y},
	m_color_text_active {color_text_active},
	m_color_text {color_text}
{
	LD("Ctor");

	int height = 0;

	for (const auto &row : options) {
		std::string id;
		if (row.size() > 1) {
			id = row.at(1);
		} else {
			id = row.at(0);
		}
		m_options.push_back(std::make_shared<WidgetImageButton>(0, height, active_pic, "", passive_pic,
			[this, id](WidgetImageButton::Event e) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::ModalYes, "MODIF" + id));
				return nullptr;
		}));

		addWidget(m_options.back());
		addWidget(std::make_shared<WidgetText>(m_options.back()->getWidth() / 2, height + m_options.back()->getHeight() / 2 + font_size / 3,
				M(row.at(0)), Align::Center, font_size, color_text_active, color_text_active, std::function<std::string(void)>{}, FontStyle::Bold));

		height += m_options.back()->getHeight();
	}
}

ModalListSelector::~ModalListSelector() {
	LD("Dtor");
}
