#ifndef WIDGETBREADCRUMBS_H_
#define WIDGETBREADCRUMBS_H_

#include "Widget.h"
#include "WidgetImageButton.h"

/// Описание элемента виджета навигации BreadCrumbs.
struct WBCElement {

	/// Текст элемента.
	std::string text;

	/// Функция обратного вызова на нажатие элемента.
	std::function<std::unique_ptr<::Event> (WidgetImageButton::Event)> on_click;

	/// Стиль элемента.
	WTFStyle style;
};


/// Виджет навигации BreadCrumbs.
class WidgetBreadCrumbs: public Widget {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param width ширина.
	 * @param height высота.
	 * @param elements элементы навигации.
	 */
	WidgetBreadCrumbs(int x, int y, int width, int height, std::vector<WBCElement> &&elements);

	/// Деструктор.
	virtual ~WidgetBreadCrumbs();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(::Event *event) override;

	/// Отрисовка изображения.
	void show() override;


private:

	std::vector<WidgetImageButtonSP> m_buttons;
	WidgetSP m_active_widget;

	WidgetSP getSelected(int x, int y);

};

#endif /* WIDGETBREADCRUMBS_H_ */
