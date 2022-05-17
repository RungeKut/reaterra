#ifndef FREERUNMENU_H_
#define FREERUNMENU_H_

#include "Menu.h"
#include "Timer.h"
#include "TreadmillDriver.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"

/// Меню Свободный бег
class FreeRunMenu: public Menu {

public:

	/// Конструктор
	FreeRunMenu();
	/// Деструктор
	virtual ~FreeRunMenu();
	/// Обработчик событий
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:
	Timer m_timer;
	bool m_lifting;
	Timer m_refresh_timer;
	int m_max_backward_speed;
	int m_max_forward_speed;
	int m_min_speed;
	int m_max_speed;
	float m_speed_step;
	bool m_paused;
	TreadmillDriver::RunState m_cur_state;

	void updateSpeedButtons();
	void onDirectionChange(bool forward);
	void doRun(bool remote = false);
	void doPause(bool remote = false);
	void doStop(bool remote = false);

	std::shared_ptr<WidgetImageButton> m_start;
	std::shared_ptr<WidgetImageButton> m_stop;
	std::shared_ptr<WidgetImageButton> m_plus_speed;
	std::shared_ptr<WidgetImageButton> m_minus_speed;
	std::shared_ptr<WidgetImageButton> m_dir_backward;
	std::shared_ptr<WidgetImageButton> m_dir_forward;
	std::shared_ptr<WidgetImageButton> m_plus_angle;
	std::shared_ptr<WidgetImageButton> m_minus_angle;

	std::shared_ptr<WidgetTextField> m_w_speed;

};

#endif /* FREERUNMENU_H_ */
