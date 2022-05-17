#ifndef MODALKEYBOARDDIGITAL_H_
#define MODALKEYBOARDDIGITAL_H_

#include "KbdEvent.h"
#include "Menu.h"

/// Цифровая клавиатура.
class ModalKeyboardDigital: public Menu {


public:

	/// Раскладка.
	enum class Layout {
		Integer,		///< Целые.
		Fixed			///< С плавающей точкой.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param prompt подсказка.
	 * @param layout раскладка.
	 * @param minus_sign наличие знака минус (true: присутствует, false: отсутствует).
	 */
	ModalKeyboardDigital(int x = 0, int y = 0, std::string prompt = "Введите число", Layout layout = Layout::Integer, bool minus_sign = false);

	/// Деструктор.
	virtual ~ModalKeyboardDigital();

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

	/// Пересчет положения.
	void updateSize();


private:

	static const int MAX_BUTTONS = 15;

	unsigned m_active;
	std::vector<KeyboardButton> m_buttons;
	std::vector<std::string> m_names;
	int m_number_buttons;
	int size;
	int gap;
	int pad;
	int inter_gap;
	uint32_t fill_color;
	uint32_t line_color;
	Layout m_layout;
	bool m_minus_sign;

	void setButton(int x, int y, int w,int n);

};


#endif /* MODALKEYBOARDDIGITAL_H_ */
