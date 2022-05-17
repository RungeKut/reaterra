#ifndef MODALCOMMONERROR_H_
#define MODALCOMMONERROR_H_

#include "Menu.h"

//TODO: Перейти на ModalCommonDialog
/// Окно сообщения об ошибке.
class ModalCommonError: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param title заголовок окна.
	 * @param message текст сообщения.
	 * @param type тип.
	 */
	ModalCommonError(std::string title, std::string message, std::string type);

	/// Деструктор.
	virtual ~ModalCommonError();
};

#endif /* MODALCOMMONERROR_H_ */
