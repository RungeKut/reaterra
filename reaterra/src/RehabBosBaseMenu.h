#ifndef REHABBOSBASEMENU_H_
#define REHABBOSBASEMENU_H_

#include "Context.h"
#include "Menu.h"
#include "WidgetImageButton.h"

/// Базовый класс для меню бега с биологической обратной связью.
class RehabBosBaseMenu : public Menu {

public:

	/// Список режимов работы.
	enum class Mode {
		MinMax,
		Power
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param mode режим работы.
	 */
	RehabBosBaseMenu(Mode mode);

	/// Деструктор.
	virtual ~RehabBosBaseMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


protected:


	/// Максимальное значение ЧСС.
	WidgetTextFieldSP m_max_possible_hr_text;

	/// Общее время процедуры.
	int m_total_time;

	/// Время разминки.
	int m_warm_time;

	/// Стиль оформления полей ввода.
	WTFStyle m_field_style;

	/// Стиль оформления названий.
	WTFStyle m_label_style;

	/// Стиль оформления единиц измерения.
	WTFStyle m_unit_style;

	/// Элемент управления "Управление нагрузкой".
	WidgetTextFieldSP m_load_control;

	/// Режим работы.
	Mode m_mode;

	/// Виджет поля ввода времени процедуры.
	WidgetTextFieldSP m_proc_time_text;

	/// Виджет заголовка времени процедуры.
	WidgetTextFieldSP m_proc_time_label;

	/// Виджет единиц измерения времени процедуры.
	WidgetTextFieldSP m_proc_time_unit_label;

	/// Виджет заголовка кнопки режима управления по углу.
	WidgetTextFieldSP m_angle_label;

	/// Виджет заголовка кнопки режима управления по скорости.
	WidgetTextFieldSP m_speed_label;

	/// Виджет заголовка кнопки режима управления по скорости и углу.
	WidgetTextFieldSP m_speed_angle_label;

	/// Виджет кнопки режима управления по скорости и углу.
	WidgetImageButtonSP m_speed_angle_btn;

	/// Виджет кнопки режима управления по углу.
	WidgetImageButtonSP m_angle_btn;

	/// Виджет кнопки режима управления по скорости.
	WidgetImageButtonSP m_speed_btn;

	/// Виджет дополнительного ряда данных.
	WidgetImageButtonSP m_extra_line;

	/// Виджет поля ввода максимальной скорости.
	WidgetTextFieldSP m_max_speed_text;

	/// Виджет поля ввода максимального угла.
	WidgetTextFieldSP m_max_angle_text;

	/// Длительность процедуры.
	int m_proc_time;

	/*!
	 * @brief Обновление времени.
	 */
	virtual void update();

	/// Структура данных коррекции.
	struct BosCorrectionData {
		int interval;
		int speed;
		int angle;
	};


private:

	Timer m_timer;
	bool m_warming_up;
	bool m_paused;
	int m_interval_cnt;
	TreadmillDriver::RunState m_cur_state;
	float m_warm_start_speed;
	float m_warm_end_speed;
	float m_warm_koef;
	int m_msg_counter;
	float m_max_speed;
	float m_n_speed;
	bool m_first_interval;


	virtual int getBosCorrectionDelta() = 0;
	virtual BosCorrectionData getBosCorrectionData(int delta) = 0;
	virtual void bosCorrection(int delta);
	virtual void onTick(){};
	virtual void onFieldChange(std::string field, float value) {};
	virtual void onMaxPossibleHrChange(int value) {};
	virtual void onStateChange(Widget::State state) {};

	WidgetImageButtonSP m_start_btn;
	WidgetImageButtonSP m_stop_btn;
	WidgetImageButtonSP m_warm_btn;
	WidgetImageButtonSP m_warm_param;
	WidgetTextFieldSP m_n_speed_text;
	WidgetTextFieldSP m_n_angle_text;
	WidgetTextFieldSP m_total_time_text;

	void stopAction();

};

#endif /* REHABBOSBASEMENU_H_ */
