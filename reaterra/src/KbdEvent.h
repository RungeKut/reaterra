#ifndef KBDEVENT_H_
#define KBDEVENT_H_

#include <string>

#include "Event.h"

/// Событие клавиатуры.
class KbdEvent: public Event {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param key название клавиши.
	 */
	KbdEvent(std::string key = "");

	/// Деструктор.
	virtual ~KbdEvent();

	/*!
	 * @brief Получение названия клавиши.
	 *
	 * @return название клавиши.
	 */
	const std::string& getKey() const;


private:

	std::string m_key;

};

#endif /* KBDEVENT_H_ */
