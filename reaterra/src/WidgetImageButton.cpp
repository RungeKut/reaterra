#include "WidgetImageButton.h"

#include <cassert>

#include "Context.h"
#include "MenuEvent.h"

WidgetImageButton::WidgetImageButton(int x, int y, std::string active_file, std::string disabled_file, std::string passive_file,
		std::function<std::unique_ptr<::Event> (Event)> on_event, Type type, WidgetTextFieldSP text)
:	Widget (x, y, State::Passive),
	m_active_id {ctx.m_pic_mgr.loadPicture(active_file)},
	m_passive_id {ctx.m_pic_mgr.loadPicture(passive_file)},
	m_disabled_id {ctx.m_pic_mgr.loadPicture(disabled_file)},
	m_on_event {on_event},
	m_type {type},
	m_trigger {false},
	m_repeat_timer {[this](int id){onRepeatTimer();}, 10},
	m_repeat_delay {0},
	m_repeat_step {0},
	m_repeat_state {Repeat::Stop},
	m_sound {SoundInterface::Sound::Click},
	m_text {text}
{
//	LD("Ctor  " + std::to_string(getX()) + " : " + std::to_string(getY()));
	cairo_surface_t *surf = nullptr;

	if (m_active_id) {
		surf = ctx.m_pic_mgr.getPicture(m_active_id);
	} else if (m_passive_id) {
		surf = ctx.m_pic_mgr.getPicture(m_passive_id);
	} else if (m_disabled_id) {
		surf = ctx.m_pic_mgr.getPicture(m_disabled_id);
	}

	if (surf) {
		setHeight(cairo_image_surface_get_height(surf));
		setWidth(cairo_image_surface_get_width(surf));
	}
}

WidgetImageButton::~WidgetImageButton() {
}

Menu* WidgetImageButton::processEvent(::Event *evt) {
	Menu *ret = nullptr;

	auto event =  dynamic_cast<TouchEvent*>(evt);

	if (!event) {
		return ret;
	}

	if (getState() == State::Disabled) {
		return ret;
	}


	int x = 0;
	int y = 0;

	if (getParent()) {
		x = getParent()->getX();
		y = getParent()->getY();
	}


	if (event->getType() == TouchEvent::TouchType::Touch) {
		setState(State::Active);

		if (m_on_event) {
			if (m_type == Type::PressRelease) {
				m_on_event(Event::Press);
			} else if (m_type == Type::Clicker) {
				if (m_repeat_delay && m_repeat_step) {
					m_repeat_state = Repeat::Delay;
					m_repeat_timer.startTimerMillis(m_repeat_delay);
				}
			}
		}
	} else {
		if (event->getType() == TouchEvent::TouchType::Untouch) {
//			LD("Width: " + std::to_string(getWidth()) + "    Height: " + std::to_string(getHeight())
//			+ "   x: " + std::to_string(getX()) + "    y: " + std::to_string(getY())
//			+ "   untouch  x: " + std::to_string(event->getX()) + "    y: " + std::to_string(event->getY()));

			if (m_on_event) {
				if (m_type == Type::Clicker) {
					if (m_repeat_state != Repeat::Run) {

						if (isSelected(event->getX() - x, event->getY() - y)) {
//							LD("Click ");
							if (m_sound != SoundInterface::Sound::Silent) {
								ctx.m_sound.play(m_sound);
							}
							auto r = std::move(m_on_event(Event::Click));
							if (r && r->getType() == ::Event::EventType::Menu) {

								MenuEvent *tmp = dynamic_cast<MenuEvent*>(r.get());
								std::unique_ptr<MenuEvent> derivedPointer;
								if(tmp != nullptr)
								{
									r.release();
									derivedPointer.reset(tmp);
								}


								ret = derivedPointer.release()->getMenu().get();
							}
						}
					}
					if (m_repeat_state != Repeat::Stop) {
						m_repeat_timer.stopTimer();
						m_repeat_state = Repeat::Stop;
					}
				} else if (m_type == Type::Trigger) {
					if (isSelected(event->getX() - x, event->getY() - y) && !m_trigger) {
						m_trigger = true;
						if (m_sound != SoundInterface::Sound::Silent) {
							ctx.m_sound.play(m_sound);
						}
						m_on_event(Event::Click);
					}
				}
			}

		}


		if (m_on_event) {
			if (m_type == Type::PressRelease) {
				m_on_event(Event::Release);
			}
		}


		if ((event->getType() == TouchEvent::TouchType::Untouch
				|| event->getType() == TouchEvent::TouchType::Clear) && getState() != State::Disabled) {
			setState(State::Passive);
		}
	}

	return ret;
}

void WidgetImageButton::show() {
	if (!isVisible()) {
		return;
	}

	int x = 0;
	int y = 0;

	if (getParent()) {
		x = getParent()->getX();
		y = getParent()->getY();
	}


	int p_id = 0;

	if (getState() == State::Active || (m_type == Type::Trigger && m_trigger)) {
		p_id = m_active_id;
	} else if (getState() == State::Passive) {
		p_id = m_passive_id;
	} else if (getState() == State::Disabled) {
		p_id = m_disabled_id;
	}

	ctx.m_graphic.drawSurface(ctx.m_pic_mgr.getPicture(p_id), x + getX(), y + getY());

	if (m_text) {
		if (getParent() != nullptr) {
			m_text->setParent(getParent());
		}
		m_text->setState(getState());
		if (isSet()) {
			m_text->setState(Widget::State::Active);
		}
		m_text->show();
	}
}

void WidgetImageButton::setPassive(std::string file) {
	m_passive_id = ctx.m_pic_mgr.loadPicture(file);
}

void WidgetImageButton::setActive(std::string file) {
	m_active_id = ctx.m_pic_mgr.loadPicture(file);
}

void WidgetImageButton::setDisabled(std::string file) {
	m_disabled_id = ctx.m_pic_mgr.loadPicture(file);
}

void WidgetImageButton::clearTrigger() {
	m_trigger = false;
}

WidgetImageButton *WidgetImageButton::setState(State state) {

//	LD("Set state: " + getStateName(state));

	if (m_type == Type::PressRelease && getState() != State::Disabled && state == State::Disabled) {
		if (m_on_event) {
			m_on_event(Event::Release);
		}
	}

	if (state != State::Active && m_repeat_state != Repeat::Stop) {
		m_repeat_timer.stopTimer();
		m_repeat_state = Repeat::Stop;
	}

	Widget::setState(state);
	return this;
}

void WidgetImageButton::setTrigger() {
	m_trigger = true;
}

bool WidgetImageButton::isSet() {
	return m_trigger;
}

void WidgetImageButton::setRepeat(int delay, int step) {
	assert(m_repeat_delay == 0);

	m_repeat_delay = delay;
	m_repeat_step = step;
}

void WidgetImageButton::onRepeatTimer() {
	if (m_repeat_state == Repeat::Delay) {
		m_repeat_timer.startTimerMillis(m_repeat_step);
		m_repeat_state = Repeat::Run;
		m_on_event(Event::Click);
		ctx.m_queue.putEvent(std::make_unique<::Event>(::Event::EventType::Refresh));
	} else if (m_repeat_state == Repeat::Run && m_on_event) {
		m_on_event(Event::Click);
		ctx.m_queue.putEvent(std::make_unique<::Event>(::Event::EventType::Refresh));
	}
}

void WidgetImageButton::setSound(SoundInterface::Sound sound) {
	m_sound = sound;
}
