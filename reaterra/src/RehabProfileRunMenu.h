#ifndef REHABPROFILERUNMENU_H_
#define REHABPROFILERUNMENU_H_

#include "Menu.h"

#include "Context.h"
#include "WidgetImageButton.h"

/// Меню "бег по профилю".
class RehabProfileRunMenu: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 */
	RehabProfileRunMenu();

	/// Деструктор.
	virtual ~RehabProfileRunMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;


private:

	DataSet m_intervals;
	DataSet m_params;
	Timer m_timer;
	WidgetImageButtonSP m_start_btn;
	WidgetImageButtonSP m_stop_btn;
	bool m_paused;
	int m_total_time;
	unsigned m_current_interval;
	int m_interval_time;
	DataSet m_intervals_data;
	float m_prev_speed;
	unsigned m_duration;
	TreadmillDriver::RunState m_cur_state;

	int getAccelTime(float speed1, float speed2, float accel);
	void stopAction();

};

#endif /* REHABPROFILERUNMENU_H_ */
