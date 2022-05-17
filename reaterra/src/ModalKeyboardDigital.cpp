#include "ModalKeyboardDigital.h"

#include "Context.h"
#include "WidgetText.h"


ModalKeyboardDigital::ModalKeyboardDigital(int x, int y, std::string prompt, Layout layout, bool minus_sign)
:	Menu {x, y},
	m_active {0},
	m_buttons(MAX_BUTTONS),
	m_names {{"7", "8", "9", "4", "5", "6", "1", "2", "3", "-", "0", ".", M("Отмена"), M("Ввод"), "←"}},
	m_number_buttons {0},
	fill_color {DEC2RGB(150, 150, 150)},
	line_color {DEC2RGB(30, 30, 30)},
	m_layout {layout},
	m_minus_sign {minus_sign}
{
	LD("Ctor");
	setType("KBDD");
	size = 85;
	gap = 5;
	pad = 5;
	inter_gap = 15;

	addWidget(std::make_shared<WidgetText>(getX() + pad + 4 * (size + gap)/* + gap / 2*/,
			getY() + pad  + 0.5 * size, prompt, Align::Center, 20, 0xAAAAAA, 0xAAAAAA,
			std::function<std::string(void)>{}, FontStyle::Normal, 2 * size + gap, 3));

	for (auto &b : m_buttons) {
		b.text_size = size / 1.8;
		b.high_fill_color = DEC2RGB(200, 200, 200);
		b.high_text_color = DEC2RGB(40, 40, 40);
		b.text_color = DEC2RGB(20, 20, 20);
	}

	updateSize();

	m_width = pad + (5) * (size + gap);
	m_height = pad + (4) * (gap + size);
}

ModalKeyboardDigital::~ModalKeyboardDigital() {
	LD("Dtor");
}

void ModalKeyboardDigital::show(bool final) {
//	LD("Show");
	ctx.m_graphic.drawRectangle(getX(), getY(), pad + (5) * (size + gap), pad + (4) * (gap + size), DEC2RGB(50, 50, 50));

	for (auto const &b : m_buttons) {
		if (!b.name.empty()) {
			ctx.m_graphic.drawKbdButton(b);
		}
	}

	for (auto &w : m_widgets) {
		w->show();
	}
}

Menu* ModalKeyboardDigital::processEvent(std::unique_ptr<Event> event) {
	if (event->getType() == Event::EventType::Touch) {
		auto tev =  dynamic_unique_cast<TouchEvent>(std::move(event));
		if (tev->getType() == TouchEvent::TouchType::Touch) {
			for (unsigned i = 0; i < m_buttons.size(); ++i) {
				if (m_buttons[i].into(tev->getX(), tev->getY())) {
					if (m_active) {
						m_buttons[m_active - 1].active = false;
					}
					m_active = i + 1;
					m_buttons[i].active = true;
					ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
					break;
				}
			}
		} else if (tev->getType() == TouchEvent::TouchType::Untouch) {
			if (m_active) {
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
				for (unsigned i = 0; i < m_buttons.size(); ++i) {
					if (m_buttons[i].into(tev->getX(), tev->getY())) {
						if (m_active == i + 1) {
//							LD("Pressed " + m_buttons[i].name);
							ctx.m_sound.play(SoundInterface::Sound::Click);
							ctx.m_queue.putEvent(std::make_unique<KbdEvent>(m_buttons[i].name));
						}
						break;
					}
				}
				m_buttons[m_active - 1].active = false;
				m_active = 0;
			}
		} else if (tev->getType() != TouchEvent::TouchType::Move) {
			if (m_active) {
				m_buttons[m_active - 1].active = false;
				m_active = 0;
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
			}
		}
	}


	return nullptr;
}


void ModalKeyboardDigital::setButton(int x, int y, int w, int n) {
	m_buttons[n].x = x;
	m_buttons[n].y = y;
	m_buttons[n].width = w;
	m_buttons[n].height = size;
	m_buttons[n].fill_color = fill_color;
	m_buttons[n].line_width = 1;
	m_buttons[n].line_color = line_color;
	m_buttons[n].name = m_names[n];
}

void ModalKeyboardDigital::updateSize() {
	for (int i = 0; i < 9; ++i) {
		setButton(getX() + pad + (i % 3) * (gap + size), getY() + pad + (i / 3) * (gap + size), size, i);
	}

	setButton(getX() + pad + 3 * (size + gap), getY() + pad + 1 * (gap + size), 2 * size + 1 * gap, 14); // Backspace
	m_buttons.at(12).text_size = size / 2.2;
	setButton(getX() + pad + 3 * (size + gap), getY() + pad + 2 * (gap + size), 2 * size + 1 * gap, 12); // Cancel
	m_buttons.at(13).text_size = size / 2.2;
	setButton(getX() + pad + 3 * (size + gap), getY() + pad + 3 * (gap + size), 2 * size + 1 * gap, 13); // Enter


	int zero_x = getX() + pad;
	int zero_w = 3 * size + 2 * gap;
	if (m_minus_sign) {
		setButton(getX() + pad, getY() + pad + 3 * (gap + size), size, 9); // Minus sign
		zero_x += size + gap;
		zero_w -= size + gap;
	}

	if (m_layout == Layout::Fixed) {
		setButton(getX() + pad + 2 * (size + gap), getY() + pad + 3 * (gap + size), size, 11); // Dot
		zero_w -= size + gap;
	}

	setButton(zero_x, getY() + pad + 3 * (gap + size), zero_w, 10); // Zero
}
