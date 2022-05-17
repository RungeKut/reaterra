#ifndef MODALKEYBOARDALPHA_H_
#define MODALKEYBOARDALPHA_H_

#include "KeyboardButton.h"
#include "KbdEvent.h"
#include "Menu.h"

/// Алфавитно-цифровая клавиатура.
class ModalKeyboardAlpha: public Menu {

public:

	/// Раскладка.
	enum class Layout {
		RusSmall,		///< Кириллица строчные.
		RusCapital,		///< Кириллица прописные.
		EngSmall,		///< Английская строчные.
		EngCapital,		///< Английская прописные.
		Digits,			///< Цифры.
		Symbols			///< Символы.
	};

	/// Расположение.
	enum class Position {
		Top,		///< Вверху.
		Bottom		///< Внизу.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param pos расположение.
	 */
	ModalKeyboardAlpha(Position pos = Position::Bottom);

	/// Деструктор.
	virtual ~ModalKeyboardAlpha();

	/*!
	 * @brief Отрисовка изображения.
	 *
	 * @param final true: финальная отрисовка на экране, false: промежуточная отрисовка в буфере.
	 */
	void show(bool final = true) override;

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

	/*!
	 * @brief Установка раскладки.
	 *
	 * @param layout раскладка.
	 */
	void setLayout(Layout layout);


private:


	static const int MAX_BUTTONS = 50;

	unsigned m_active;
	std::vector<KeyboardButton> m_buttons;
	std::array<std::vector<std::string>, 5> m_names;
	int m_number_buttons;
	Layout m_layout;
	int size;
	int gap;
	int pad;
	int inter_gap;
	Layout m_prev_layout;
	uint32_t fill_color;
	uint32_t line_color;

	void setButton(int x, int y, int w,int n);
};

#endif /* MODALKEYBOARDALPHA_H_ */
