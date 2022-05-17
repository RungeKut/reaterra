#ifndef WIDGETPROFIL_H_
#define WIDGETPROFIL_H_

#include "Widget.h"

/// Виджет отображения графиков профиля.
class WidgetProfil: public Widget {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param points точки графиков.
	 */
	WidgetProfil(int x, int y, std::vector<std::tuple<unsigned, float, float>> points);

	/// Деструктор.
	virtual ~WidgetProfil();

private:

	std::vector<std::tuple<unsigned, float, float>> m_points;
	void show() override;

};

#endif /* WIDGETPROFIL_H_ */
