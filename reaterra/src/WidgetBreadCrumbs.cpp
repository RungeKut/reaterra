#include "WidgetBreadCrumbs.h"

#include "Context.h"
#include "WidgetTextField.h"

WidgetBreadCrumbs::WidgetBreadCrumbs(int x, int y, int width, int height, std::vector<WBCElement> &&elements)
:	Widget (x, y, State::Passive)
{
	setWidth(width);
	setHeight(height);
	int xs = x;
	unsigned cnt = 0;
	bool out = false;
	for (const auto e : elements) {
		if (xs > x + width - 40) {
			break;
		}
		SizeGraph size = ctx.m_graphic.getTextSize(e.text, e.style.font_size, e.style.font_style);
		size.width += 2 * e.style.hpad;
		if (xs + size.width >  x + width) {
			size.width = x + width - xs;
			out = true;
		}
		WidgetTextFieldSP text = std::make_shared<WidgetTextField>(xs, y, size.width, height, e.style, e.text);
		m_buttons.push_back(std::make_shared<WidgetImageButton>(xs, y, "", "", "", e.on_click, WidgetImageButton::Type::Clicker, text));
		m_buttons.back()->setWidth(size.width);
		m_buttons.back()->setHeight(height);
		if (!e.on_click) {
			m_buttons.back()->setState(State::Disabled);
		}
		if (out) {
			break;
		}
		xs += size.width;
		if (++cnt < elements.size()) {
			m_buttons.push_back(std::make_shared<WidgetImageButton>(xs, y, "", "breadcrumbs_separator.png"));
			m_buttons.back()->setState(State::Disabled);
			xs += m_buttons.back()->getWidth();
		}
	}
}

WidgetBreadCrumbs::~WidgetBreadCrumbs() {
}

Menu* WidgetBreadCrumbs::processEvent(::Event* event) {

	Menu* ret = nullptr;
	if (event->getType() == Event::EventType::Touch) {
		if (event->getType() == Event::EventType::Touch){

			auto tev =  dynamic_cast<TouchEvent*>(event);
//				LD("  TouchEvent x: " + std::to_string(tev->getX()) + "    y: " + std::to_string(tev->getY()));

			if (m_active_widget.get()) {
				ret = m_active_widget->processEvent(event);
				if (tev->getType() != TouchEvent::TouchType::Move) {
					m_active_widget = nullptr;
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
				}
			} else {
				WidgetSP w = getSelected(tev->getX(), tev->getY());
				if (w.get() != nullptr) {
					if (tev->getType() == TouchEvent::TouchType::Touch) {
						m_active_widget = w;
						ret = w->processEvent(event);
						ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
					}
				}
			}
		}
	}
	return ret;
}

void WidgetBreadCrumbs::show() {
	for (auto &w : m_buttons) {
		w->show();
	}
}

WidgetSP WidgetBreadCrumbs::getSelected(int x, int y) {
	WidgetSP ret;

	for (auto w : m_buttons) {
		if (w->getState() == Widget::State::Disabled) {
			continue;
		}
		if (w->isSelected(x, y)) {
			ret = w;
			break;
		}
	}

	return ret;
}
