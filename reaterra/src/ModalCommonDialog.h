#ifndef MODALCOMMONDIALOG_H_
#define MODALCOMMONDIALOG_H_

#include "Menu.h"

#include "EnumExtension.h"
#include "WidgetTextField.h"

/// Универсальный диалог.
class ModalCommonDialog: public Menu {

public:

	/// Флаги.
	enum class Flags : unsigned {
		GreenTitle = 1, 		///< Зеленый заголовок.
		YellowTitle = 2,		///< Желтый заголовок.
		NoSound = 8,			///< Не использовать звук.
		OkButton = 0x10,		///< Кнопка ОК.
		CloseButton = 0x20,		///< Кнопка ЗАКРЫТЬ.
		YesCancel = 0x40,		///< Кнопки ДА, ОТМЕНА.
		YesNoCancel = 0x80,		///< Кнопки ДА, НЕТ, ОТМЕНА.
		Yes = 0x100,			///< Кнопка ДА.
		Cancel = 0x200,		    ///< Кнопка ОТМЕНА.
		EmptyButton = 0x400,		    ///< Кнопка без текста.
		StopIcon = 0x1000,		///< Иконка СТОП.
		FeetIcon = 0x2000,		///< Иконка НОГИ.
		IconClickable = 0x8000, ///< Возможность нажатия иконки в заголовке.
		Buttons = 0xFF0,		///< Маска КНОПКИ.
		Icons = 0xF000			///< Маска ИКОНКИ.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param title заголовок диалога.
	 * @param message текст сообщения.
	 * @param type тип.
	 * @param flags флаги.
	 * @param icon_file иконка.
	 */
	ModalCommonDialog(std::string title, std::string message, std::string type, Flags flags, std::string icon_file, std::string button_text = "");

	/// Деструктор.
	virtual ~ModalCommonDialog();

	/*!
	 * @brief Установка таймера.
	 *
	 * @param period длительность периода, мс.
	 * @param on_timer функция обработчик события таймера.
	 * @param id идентификатор таймера.
	 */
	void setTimer(int period, TimerFunc on_timer, int id = 0, int count = 0);

	/*!
	 * @brief Установка текста сообщения.
	 *
	 * @param text текст сообщения.
	 */
	void setText(std::string text);

private:

	Flags m_flags;
	int m_msg_counter = 0;
	std::unique_ptr<Timer> m_timer;
	WidgetTextFieldSP m_text;

};

ENABLE_BITMASK_OPERATORS(ModalCommonDialog::Flags)

#endif /* MODALCOMMONDIALOG_H_ */
