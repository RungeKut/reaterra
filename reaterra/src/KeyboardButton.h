#ifndef KEYBOARDBUTTON_H_
#define KEYBOARDBUTTON_H_

#include <cstdint>
#include <array>
#include <string>

/// Клавиша клавиатуры.
class KeyboardButton {

public:

	int x;						///< Координата x.
	int y;						///< Координата y.
	int width;					///< Ширина.
	int height;					///< Высота.
	uint32_t fill_color;		///< Цвет заполнения.
	int line_width;				///< Толщина линии.
	uint32_t line_color;		///< Цвет линии.
	uint32_t high_fill_color;	///< Активный цвет заполнения.
	uint32_t high_text_color;	///< Активный цвет текста.
	uint32_t text_color;		///< Цвет текста.
	int text_size;				///< Размер текста.
	std::string name;			///< Название клавиши.
	bool active;				///< Флаг активности.

	/*!
	 * @brief Определение нажатия.
	 *
	 * @param xt координата x касания.
	 * @param yt координата y касания.
	 * @return true: есть нажатие, false: нет нажатия.
	 */
	bool into(int xt, int yt);

};

#endif /* KEYBOARDBUTTON_H_ */
