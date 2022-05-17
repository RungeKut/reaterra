#include "ModalKeyboardAlpha.h"

#include "Context.h"
#include "KbdEvent.h"

ModalKeyboardAlpha::ModalKeyboardAlpha(Position pos)
:	Menu {0, pos == Position::Top ? 0 : 435},
	m_active {0},
	m_buttons(MAX_BUTTONS),
	m_names {{
		{"й", "ц", "у", "к", "е", "н", "г", "ш", "щ", "з", "х",
		 "ф", "ы", "в", "а", "п", "р", "о", "л", "д", "ж", "э",
		 "^", "я", "ч", "с", "м", "и", "т", "ь", "б", "ю", "←",
		 "&!", "En", " ", M("Отмена"), M("Ввод"), "7", "8", "9", "4", "5", "6", "1", "2", "3", ",", "0", "."},
		{"Й", "Ц", "У", "К", "Е", "Н", "Г", "Ш", "Щ", "З", "Х", "Ф", "Ы", "В", "А", "П", "Р", "О", "Л", "Д", "Ж", "Э", "^", "Я", "Ч", "С", "М", "И", "Т", "Ь", "Б", "Ю", "←", "&!", "En", " ", M("Отмена"), M("Ввод")},
		{"q", "w", "e", "r", "t", "y", "u", "i", "o", "p",  "", "a", "s", "d", "f", "g", "h", "j", "k", "l",  "",  "", "^", "z", "x", "c", "v", "b", "n", "m",  "←",  "", "", "&!", "Ру", " ", M("Отмена"), M("Ввод")},
		{"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",  "", "A", "S", "D", "F", "G", "H", "J", "K", "L",  "",  "", "^", "Z", "X", "C", "V", "B", "N", "M",  "←",  "", "", "&!", "Ру", " ", M("Отмена"), M("Ввод")},
		{"`", "!", "#", "$", "%", "&", "*", "(", ")", "-", "+", "@", ":", ";", "'", "\"", "<", ">", "{", "}", "/", "\\", "^", "?", "=", "_", M("№"), "~", "|", M("ё"), M("Ъ"), M("ъ"), "←", "Ab", "", " ", M("Отмена"), M("Ввод")}
	}},
	m_number_buttons {0},
	m_prev_layout {Layout::RusSmall},
	fill_color {DEC2RGB(150, 150, 150)},
	line_color {DEC2RGB(30, 30, 30)}
{
	LD("Ctor");
	setType("KBDA");
	size = 85;
	gap = 5;
	pad = 5;
	inter_gap = 15;

	for (auto &b : m_buttons) {
		b.text_size = size / 1.8;
		b.high_fill_color = DEC2RGB(200, 200, 200);
		b.high_text_color = DEC2RGB(40, 40, 40);
		b.text_color = DEC2RGB(20, 20, 20);
	}

	if (Locale::instance().getLocale() == "RU") {
		setLayout(Layout::RusSmall);
	} else {
		for (auto &k : m_names) {
			k[34] = "";
		}
		setLayout(Layout::EngSmall);
	}
}

ModalKeyboardAlpha::~ModalKeyboardAlpha() {
	LD("Dtor");
}

void ModalKeyboardAlpha::show(bool final) {
//	LD("Show");
	Menu::show(false);
//	LD("Rectangle");
	ctx.m_graphic.drawRectangle(getX(), getY(), inter_gap + pad + (14) * (size + gap), pad + (4) * (gap + size), DEC2RGB(50, 50, 50));

	for (auto const &b : m_buttons) {
		if (!b.name.empty()) {
			ctx.m_graphic.drawKbdButton(b);
		}
	}
}

Menu* ModalKeyboardAlpha::processEvent(std::unique_ptr<Event> event) {
	if (event->getType() == Event::EventType::Touch) {
		auto tev =  dynamic_unique_cast<TouchEvent>(std::move(event));
		if (tev->getType() == TouchEvent::TouchType::Touch) {
//			LD("touch: x-" + std::to_string(tev->getX()) + "   y-" + std::to_string(tev->getY()));
			for (unsigned i = 0; i < m_buttons.size(); ++i) {
				if (m_buttons[i].into(tev->getX(), tev->getY())) {
//					LD("btn found: " + std::to_string(i) + "  " + m_buttons[i].name);
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
							ctx.m_sound.play(SoundInterface::Sound::Click);
//							LD("Pressed " + m_buttons[i].name);
							if (m_buttons[i].name == "^" && m_layout != Layout::Symbols) {
								if (m_layout == Layout::RusSmall) {
									setLayout(Layout::RusCapital);
								} else if (m_layout == Layout::RusCapital) {
									setLayout(Layout::RusSmall);
								} else if (m_layout == Layout::EngCapital) {
									setLayout(Layout::EngSmall);
								} else if (m_layout == Layout::EngSmall) {
									setLayout(Layout::EngCapital);
								}
							} else if (m_buttons[i].name == "En") {
								if (m_layout == Layout::RusSmall) {
									setLayout(Layout::EngSmall);
								} else if (m_layout == Layout::RusCapital) {
									setLayout(Layout::EngCapital);
								}
							} else if (m_buttons[i].name == "Ру") {
								if (m_layout == Layout::EngSmall) {
									setLayout(Layout::RusSmall);
								} else if (m_layout == Layout::EngCapital) {
									setLayout(Layout::RusCapital);
								}
							} else if (m_buttons[i].name == "&!") {
								m_prev_layout = m_layout;
								setLayout(Layout::Symbols);
							} else if (m_buttons[i].name == "Ab") {
								m_layout = m_prev_layout;
								setLayout(m_layout);
							} else if (m_buttons[i].name == "\\") {
								ctx.m_queue.putEvent(std::make_unique<KbdEvent>("¦"));
							} else {
//							LD("Put event");
								ctx.m_queue.putEvent(std::make_unique<KbdEvent>(m_buttons[i].name));
							}

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

void ModalKeyboardAlpha::setLayout(Layout layout) {
	m_layout = layout;

	float width = size;
	if (m_layout == Layout::EngSmall || m_layout == Layout::EngCapital) {
		width = size + (size + gap) / 10.0;
	}

	for (int i = 0; i < 11; ++i) {
		setButton(getX() + pad + i * (width + gap), getY() + pad, width, i);
	}

	if (m_layout == Layout::EngSmall || m_layout == Layout::EngCapital) {
		width = size + 2 * (size + gap) / 9.0;
	}
	for (int i = 11; i < 22; ++i) {
		setButton(getX() + pad + (i - 11) * (width + gap), getY() + pad + gap + size, width, i);
	}

	for (int i = 22; i < 33; ++i) {
		setButton(getX() + pad + (i - 22) * (width + gap), getY() + pad + 2 * (gap + size), width, i);
	}

	if (m_layout == Layout::Symbols || Locale::instance().getLocale() != "RU") {
		width = 2 * size + gap;
	} else {
		width = size;
	}
	setButton(getX() + pad, getY() + pad + 3 * (gap + size), width, 33); // symbols
	setButton(getX() + pad + size + gap, getY() + pad + 3 * (gap + size), size, 34); // Ru/En
	setButton(getX() + pad + 2 * (gap + size), getY() + pad + 3 * (gap + size), 3 * size + 2 * gap, 35); // Space
	setButton(getX() + pad + 5 * (gap + size), getY() + pad + 3 * (gap + size), 3 * size + 2 * gap, 36);  // Cancel
	setButton(getX() + pad + 8 * (gap + size), getY() + pad + 3 * (gap + size), 3 * size + 2 * gap, 37); // Enter

	// Digital keyboard
	for (int i = 38; i < 50; ++i) {
		setButton(getX() + inter_gap + pad + (11 + (i - 38) % 3) * (gap + size), getY() + pad + ((i - 38) / 3) * (gap + size), size, i);
	}
}

void ModalKeyboardAlpha::setButton(int x, int y, int w, int n) {
	m_buttons[n].x = x;
	m_buttons[n].y = y;
	m_buttons[n].width = w;
	m_buttons[n].height = size;
	m_buttons[n].fill_color = fill_color;
	m_buttons[n].line_width = 1;
	m_buttons[n].line_color = line_color;

	if (m_layout == Layout::RusSmall || n > 37) {
		m_buttons[n].name = m_names[0][n];
	} else if (m_layout == Layout::RusCapital) {
		m_buttons[n].name = m_names[1][n];
	} else if (m_layout == Layout::EngSmall){
		m_buttons[n].name = m_names[2][n];
	} else if (m_layout == Layout::EngCapital){
		m_buttons[n].name = m_names[3][n];
	} else if (m_layout == Layout::Symbols){
		m_buttons[n].name = m_names[4][n];
	}

//	LD("btn: x-" + std::to_string(m_buttons[n].x)
//		+"  y-" + std::to_string(m_buttons[n].y)
//	+"  w-" + std::to_string(m_buttons[n].width)
//	+"  h-" + std::to_string(m_buttons[n].height)
//	+"  n-" + m_buttons[n].name
//	);
}
