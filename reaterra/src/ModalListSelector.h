#ifndef MODALLISTSELECTOR_H_
#define MODALLISTSELECTOR_H_

#include "DbConnector.h"
#include "Menu.h"
#include "WidgetImageButton.h"

/// Окно выбора из списка.
class ModalListSelector: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param options элементы списка.
	 * @param active_pic фон активного элемента.
	 * @param passive_pic фон пассивного элемента.
	 * @param color_text_active цвет текста активного элемента.
	 * @param color_text цвет текста пассивного элемента.
	 * @param font_size размер шрифта.
	 */
	ModalListSelector(int x, int y, DataSet options, std::string active_pic, std::string passive_pic, uint32_t color_text_active = 0x3C3C3C,
			uint32_t color_text = 0x3C3C3C, int font_size = 24);

	/// Деструктор.
	virtual ~ModalListSelector();

private:

	uint32_t m_color_text_active;
	uint32_t m_color_text;

	std::vector<std::shared_ptr<WidgetImageButton>> m_options;
};

#endif /* MODALLISTSELECTOR_H_ */
