#ifndef WIDGETTEXT_H_
#define WIDGETTEXT_H_

#include <functional>

#include "Widget.h"
#include "WidgetImageButton.h"

#include "Menu.h"

/// Виджет однострочного текста.
class WidgetText: public Widget {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param text отображаемый текст.
	 * @param align выравнивание.
	 * @param size размер шрифта.
	 * @param color цвет текста.
	 * @param color_disabled цвет заблокированного текста.
	 * @param on_update функция, вызываемая при отображении виджета.
	 * @param font_style стиль шрифта.
	 * @param width ширина текста.
	 * @param lines количество строк.
	 * @param v_align вертикальное выравнивание.
	 * @param dy расстояние между строками.
	 */
	WidgetText(int x, int y, std::string text, Align align = Align::Left, int size = 30,
			uint32_t color = 0, uint32_t color_disabled = 0x808080, std::function<std::string(void)> on_update = {},
			FontStyle font_style = FontStyle::Normal, int width = 0, int lines = 1, Align v_align = Align::Center, int dy = 0);

	/// Деструктор.
	virtual ~WidgetText();

	/// Отрисовка изображения.
	void show() override;

	/*!
	 * @brief Получение вида горизонтального выравнивания.
	 *
	 * @return вид горизонтального выравнивания.
	 */
	Align getAlign() const;

	/*!
	 * @brief Установка вида горизонтального выравнивания.
	 *
	 * @param align вид горизонтального выравнивания.
	 */
	void setAlign(Align align);

	/*!
	 * @brief Получение цвета текста.
	 *
	 * @return цвет текста.
	 */
	uint32_t getColor() const;

	/*!
	 * @brief Получение цвета заблокированного текста.
	 *
	 * @return цвет заблокированного текста.
	 */
	uint32_t getColorDisabled() const;

	/*!
	 * @brief Получение текста виджета.
	 *
	 * @return текст виджета.
	 */
	const std::string& getText() const;

	/*!
	 * Установка текста виджета.
	 *
	 * @param text текст виджета.
	 */
	void setText(const std::string& text);


private:

	std::string m_text;
	Align m_align;
	int m_font_size;
	uint32_t m_color;
	uint32_t m_color_disabled;
	std::function<std::string(void)> m_on_update;
	FontStyle m_font_style;
	int m_lines;
	Align m_v_align;
	int m_dy;
	std::shared_ptr<WidgetImageButton> m_back_button;

	void setColor(uint32_t color);
	int getFontSize() const;
	void setFontSize(int fontSize);

};


#endif /* WIDGETTEXT_H_ */
