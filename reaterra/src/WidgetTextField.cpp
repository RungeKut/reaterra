#include "WidgetTextField.h"

#include "Context.h"
#include "Menu.h"
#include "ModalKeyboardAlpha.h"
#include "ModalKeyboardDigital.h"
#include "Utils.h"



WidgetTextField::WidgetTextField(int x, int y, int width, int height, WTFStyle style, std::string text, std::string prompt, CheckFunc checkfunc)
: Widget(x, y, State::Disabled),
  m_style {style},
  m_trigger {false},
  m_id_row {0},
  m_id_col {0},
  m_active_picture_id {style.img_file_active.empty() ? 0 : ctx.m_pic_mgr.loadPicture(style.img_file_active)},
  m_passive_picture_id {style.img_file_passive.empty() ? 0 : ctx.m_pic_mgr.loadPicture(style.img_file_passive)},
  m_disabled_picture_id {style.img_file_disabled.empty() ? 0 : ctx.m_pic_mgr.loadPicture(style.img_file_disabled)}
{


	if (!text.empty()) {
		m_style.text = text;
	}

	SizeGraph size;
	if (!width || !height) {
		size = ctx.m_graphic.getTextSize(m_style.text.empty() ? " " : m_style.text, m_style.font_size, m_style.font_style);
	}

	setWidth(width ? width : size.width);
	setHeight(height ? height : size.height);
}

WidgetTextField::~WidgetTextField() {
}

void WidgetTextField::show() {
	if (!isVisible()) {
		return;
	}

	if (getParent() && !getParent()->m_modal) {
		m_trigger = false;
		m_kbd_focus = false;
	}

	if (m_style.update_func) {
		m_style.text = m_style.update_func(*this);
	}

	if (m_style.text.empty() && getState() == State::Disabled) {
		return;
	}

	int x = 0;
	int y = 0;

	if (getParent()) {
		x = getParent()->getX();
		y = getParent()->getY();
	}

	uint32_t c;

	if (getState() == State::Active || m_trigger) {
//		LD("Draw: x: " + std::to_string(x + getX()) + "  y:  " + std::to_string(y + getY()));
		if (m_active_picture_id) {
			ctx.m_graphic.drawSurface(ctx.m_pic_mgr.getPicture(m_active_picture_id), x + getX(), y + getY());
		} else if (m_style.rect_color_active < to_base(Colors::White)) {
			ctx.m_graphic.drawRectangle(x + getX(), y + getY(), getWidth(), getHeight(), m_style.rect_color_active);
		}
		c = m_style.text_color_active;
	} else if (getState() == State::Disabled/* || m_style.kbd_type == KbdType::None*/) {
		c = m_style.text_color_disabled;
		if (m_style.rect_color_disabled < to_base(Colors::White)) {
			ctx.m_graphic.drawRectangle(x + getX(), y + getY(), getWidth(), getHeight(), m_style.rect_color_disabled);
		}
	} else {
		c = m_style.text_color_passive;
		if (m_style.rect_color_passive < to_base(Colors::White)) {
			ctx.m_graphic.drawRectangle(x + getX(), y + getY(), getWidth(), getHeight(), m_style.rect_color_passive);
		}
	}

	if (!m_trigger) {
		if (getState() == State::Passive && m_passive_picture_id) {
			ctx.m_graphic.drawSurface(ctx.m_pic_mgr.getPicture(m_passive_picture_id), x + getX(), y + getY());
		} else if (getState() == State::Disabled && m_disabled_picture_id) {
			ctx.m_graphic.drawSurface(ctx.m_pic_mgr.getPicture(m_disabled_picture_id), x + getX(), y + getY());
		}
	}



	if (m_style.halign == Align::Center) {
		x += getWidth() / 2;
	} else if (m_style.halign == Align::Right) {
		x += getWidth() - 2 * m_style.hpad;
	}

	if (m_style.valign == Align::Center) {
		y += getHeight() / 2;
	}

	std::string text = m_style.text;

	if (m_kbd_focus) {
		if (m_style.password_char.empty()) {
			text = Utils::wstring_to_utf8(m_editor_string);
		} else {
			text = "";
			for (unsigned i = 0; i < m_editor_string.size(); ++i) {
				text += "\u2022";
			}
		}
	} else {
		if (!m_style.password_char.empty()) {
			text = "";
			for (unsigned i = 0; i < m_style.text.size(); ++i) {
				text += "\u2022";
			}
		}
	}

	m_style.dy = m_style.dy ? m_style.dy : 1.2 * m_style.font_size;
//	LD("Show " + std::to_string(getWidth()) + "   " + std::to_string(y + getY()) + "  "+ text);
	int hpad = m_style.halign == Align::Center ? 0 : m_style.hpad;
	ctx.m_graphic.drawMultiText(text, x + getX() + hpad, y + getY(), getWidth() - 2 * hpad, m_style.lines, m_style.dy, m_style.font_size,
			m_style.halign, m_style.valign, m_style.font_style, c, m_style.text_rotate);
}

WidgetTextField &WidgetTextField::setText(const std::string& text, const std::string &id) {
	m_style.text = Utils::allTrim(text);
	m_editor_string = Utils::utf8_to_wstring(m_style.text);
	m_text_id = id;
	return *this;
}

WidgetTextField& WidgetTextField::setUpdateFunc(UpdateFunc update_func) {
	m_style.update_func = update_func;
	return *this;
}

WidgetTextField& WidgetTextField::setCheckFunc(CheckFunc check_func) {
	m_style.check_func = check_func;
	return *this;
}

WidgetTextField& WidgetTextField::setPrompt(std::string prompt) {
	m_style.prompt = prompt;
	return *this;
}

WidgetTextField& WidgetTextField::setLimits(float min, float max, std::string format) {
	m_style.min_limit = min;
	m_style.max_limit = max;
	m_style.limit_format = format;
	return *this;
}

Menu* WidgetTextField::processEvent(::Event *evt) {
	Menu *ret = nullptr;

	if (getState() == State::Disabled || !isVisible()) {
		return ret;
	}

//	LD("Event is " + evt->getName() + "  extra: " + evt->getExtra() + "    text: [" + m_style.text + "]");
	if (evt->getType() == Event::EventType::KbdEvent) {
		auto kev =  dynamic_cast<KbdEvent*>(evt);
		if (kev) {
			std::string s = kev->getKey();
			if (s == M("Отмена")) {
				getParent()->m_modal.reset();
				m_trigger = false;
				m_kbd_focus = false;
			} else if (s == M("Ввод")) {

				std::string result = Utils::wstring_to_utf8(m_editor_string);

//				LD("Before test kbd event " + std::to_string(result.size()) + "[" + result + "] code " + (result.size()?Utils::int2Hex(result.at(0)):""));

				result = Utils::allTrim(result);



				if (m_style.kbd_type && KbdType::Digital
						&& (result.empty()
						|| (m_style.min_limit != m_style.max_limit && (std::stof(result) < m_style.min_limit || std::stof(result) > m_style.max_limit)))) {
					return ret;
				}

				if ((m_style.kbd_type && KbdType::Digital) && !m_style.num_format.empty()) {
					result = Utils::formatFloat(m_style.num_format, std::stof(result));
					m_editor_string = Utils::utf8_to_wstring(result);
				}


				if (m_style.check_func) {
					std::string ret = m_style.check_func(result, *this);
					result = Utils::wstring_to_utf8(m_editor_string);
//					LD("ret :  " + ret + "   " + (m_style.kbd_type && KbdType::Digital ? "true" : "false"));
					if (ret.empty()) {
						m_style.text = result;
					} else if ((m_style.kbd_type && KbdType::Digital) && ret.at(0) == '@') {
						m_style.text = ret.substr(1);
					}
				} else {
					m_style.text = result;
				}
					getParent()->m_modal.reset();
					m_trigger = false;
					m_kbd_focus = false;
			} else if (s == "←") {
				if (m_editor_string.size() > 0) {
					m_editor_string.pop_back();
				}
			} else if (!s.empty() && static_cast<int>(m_editor_string.size()) < m_style.text_max_size) {
				if (m_style.kbd_type && KbdType::Date) {
					if (m_editor_string.size() < 10 && ((m_editor_string.size() != 4 && m_editor_string.size() != 7 && s != ".")
							|| ((m_editor_string.size() == 4 || m_editor_string.size() == 7) && s == "."))) {
						m_editor_string += Utils::utf8_to_wstring(s);
					}
				} else if ((s != "." && s != "-") || (m_style.kbd_type && KbdType::Alpha)
						|| (s == "-" && (m_style.kbd_type && KbdType::Digital) && m_editor_string.empty())
						|| (s == "." && (m_style.kbd_type && KbdType::Digital) && !m_editor_string.empty()
								&& m_editor_string.find_first_of(Utils::utf8_to_wstring(".")) == m_editor_string.npos)) {
					m_editor_string += Utils::utf8_to_wstring(s);
				}
			}

			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
//		LD("Event exit text: [" + m_style.text + "]");
		return nullptr;
	}

	int x = 0;
	int y = 0;

	if (getParent()) {
		x = getParent()->getX();
		y = getParent()->getY();
	}


	auto event =  dynamic_cast<TouchEvent*>(evt);

	if (!event) {
		return ret;
	}

	if (event->getType() == TouchEvent::TouchType::Touch) {
		setState(State::Active);
	} else {
		if (event->getType() == TouchEvent::TouchType::Untouch) {
//			LD("Width: " + std::to_string(getWidth()) + "    Height: " + std::to_string(getHeight())
//			+ "   x: " + std::to_string(getX()) + "    y: " + std::to_string(getY())
//			+ "   untouch  x: " + std::to_string(event->getX()) + "    y: " + std::to_string(event->getY()));


			if (isSelected(event->getX() - x, event->getY() - y) && !m_trigger) {
				m_trigger = true;
				m_kbd_focus = true;
				m_editor_string = Utils::utf8_to_wstring(m_style.text);
				ctx.m_sound.play(SoundInterface::Sound::Click);
				if (m_style.kbd_type && KbdType::Alpha) {
					getParent()->m_modal = std::make_unique<ModalKeyboardAlpha>(getY() + y > SCREEN_HEIGHT / 2 ? ModalKeyboardAlpha::Position::Top : ModalKeyboardAlpha::Position::Bottom);
				} else if ((m_style.kbd_type && KbdType::Digital) || (m_style.kbd_type && KbdType::Date)) {
					ModalKeyboardDigital::Layout layout;
					m_editor_string = std::wstring();
					if ((m_style.kbd_type && KbdType::DotFlag) || (m_style.kbd_type && KbdType::Date)) {
						layout = ModalKeyboardDigital::Layout::Fixed;
					} else {
						layout = ModalKeyboardDigital::Layout::Integer;
					}
					bool minus_flag = m_style.kbd_type && KbdType::MinusFlag;
					std::string prompt = m_style.prompt;
					if (m_style.min_limit != m_style.max_limit) {
						char buff[200];
						if (m_style.limit_format.empty()) {
							sprintf(buff, M("Ввод числа\\от %g до %g").c_str(), m_style.min_limit, m_style.max_limit);
						} else {
							sprintf(buff, m_style.limit_format.c_str(), m_style.min_limit, m_style.max_limit);
						}
						prompt = buff;
					}

					if (m_style.kbd_type && KbdType::Date) {
						prompt = M("Ввод даты\\ГГГГ.ММ.ДД");
					}

					std::unique_ptr<ModalKeyboardDigital> kbd = std::make_unique<ModalKeyboardDigital>(0, 0, prompt, layout, minus_flag);
					int xk = x + getX() + getWidth() / 2 - kbd->getWidth() / 2;
					if (xk + kbd->getWidth() > SCREEN_WIDTH) {
						xk = SCREEN_WIDTH - kbd->getWidth();
					} else if (xk < 0) {
						xk = 0;
					}
					int yk = getY() + y > SCREEN_HEIGHT / 2 ? getY() + y - kbd->getHeight() - 10 : getY() + y + getHeight() + 10;

					if (yk + kbd->getHeight() > SCREEN_HEIGHT) {
						yk = SCREEN_HEIGHT - kbd->getHeight();
					} else if (yk < 0) {
						yk = 0;
					}


					kbd->setX(xk);
					kbd->setY(yk);
					kbd->updateSize();

					getParent()->m_modal = std::move(kbd);
				} else {
					m_trigger = false;
					m_kbd_focus = false;

					if (m_style.check_func) {
						std::string ret = m_style.check_func("", *this);
					}

				}
			}
		}


		if ((event->getType() == TouchEvent::TouchType::Untouch
				|| event->getType() == TouchEvent::TouchType::Clear) && getState() != State::Disabled) {
			LD("Set passive on untouch");
			setState(State::Passive);
		}
	}


//	ret = Menu::processEvent(std::move(event));

	return ret;
}

int WidgetTextField::getIdCol() const {
	return m_id_col;
}

WidgetTextField &WidgetTextField::setIdCol(int idCol) {
	m_id_col = idCol;
	return *this;
}

int WidgetTextField::getIdRow() const {
	return m_id_row;
}

std::string WidgetTextField::getText() const {
	return m_style.text;
}

WidgetTextField& WidgetTextField::setMaxLimit(float max) {
	m_style.max_limit = max;
	return *this;
}

WidgetTextField &WidgetTextField::setIdRow(int idRow) {
	m_id_row = idRow;
	return *this;
}

std::string WidgetTextField::getTextId() const {
	return m_text_id;
}

void WidgetTextField::setFontSize(unsigned size) {
	m_style.font_size = size;
}
