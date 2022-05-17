#ifndef FREERUNMENUSAS_H_
#define FREERUNMENUSAS_H_

#include "Menu.h"
#include "Timer.h"
#include "TreadmillDriver.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"


/// Меню Свободный бег
class FreeRunMenuSas: public Menu {


public:

	/// Список режимов работы.
	enum class SasMode {
		NoSas = 0,
		SasFreeWalk = 1,
		SasTest = 2,
		SasExo = 3
	};

	/// Конструктор
	FreeRunMenuSas();

	/// Деструктор
	virtual ~FreeRunMenuSas();

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
	int m_hr_max;
	int m_hr_crit;
	int m_hr_alarm;
	SasMode m_sas_mode;
	bool m_remote_pult = true;
	bool m_start_left_show = true;

	void updateSpeedButtons();
	void onDirectionChange(bool forward);
	void doRun(bool remote = false);
	void doPause(bool remote = false);
	void doStop(bool remote, bool info = false);
	void sasModeUpdate(SasMode smode);
	void sasLimitationUpdate();

	std::shared_ptr<WidgetImageButton> m_start;
	std::shared_ptr<WidgetImageButton> m_stop;
	std::shared_ptr<WidgetImageButton> m_plus_speed;
	std::shared_ptr<WidgetImageButton> m_minus_speed;
	std::shared_ptr<WidgetImageButton> m_dir_backward;
	std::shared_ptr<WidgetImageButton> m_dir_forward;
	std::shared_ptr<WidgetImageButton> m_plus_angle;
	std::shared_ptr<WidgetImageButton> m_minus_angle;
	std::shared_ptr<WidgetImageButton> m_mode_free_run;
	std::shared_ptr<WidgetImageButton> m_mode_free_walk;
	std::shared_ptr<WidgetImageButton> m_mode_6m_test;
	std::shared_ptr<WidgetImageButton> m_mode_exo;
	std::shared_ptr<WidgetImageButton> m_div;
	std::shared_ptr<WidgetImageButton> m_div_whole;
	std::shared_ptr<WidgetImageButton> m_div_half;
	std::shared_ptr<WidgetImageButton> m_power_pic;
	std::shared_ptr<WidgetImageButton> m_simmetry_pic;
	std::shared_ptr<WidgetImageButton> m_step_pic;
	std::shared_ptr<WidgetImageButton> m_hr_settings;

	std::shared_ptr<WidgetTextField> m_w_speed;
	std::shared_ptr<WidgetTextField> m_power_lbl;
	std::shared_ptr<WidgetTextField> m_power_mech_lbl;
	std::shared_ptr<WidgetTextField> m_power_aero_lbl;
	std::shared_ptr<WidgetTextField> m_power_mech_value;
	std::shared_ptr<WidgetTextField> m_power_aero_value;
	std::shared_ptr<WidgetTextField> m_simmetry_lbl;
	std::shared_ptr<WidgetTextField> m_step_lbl;
	std::shared_ptr<WidgetTextField> m_simmetry_dim;
	std::shared_ptr<WidgetTextField> m_step_dim;
	std::shared_ptr<WidgetTextField> m_simmetry_left;
	std::shared_ptr<WidgetTextField> m_simmetry_right;
	std::shared_ptr<WidgetTextField> m_simmetry_left_value;
	std::shared_ptr<WidgetTextField> m_simmetry_right_value;
	std::shared_ptr<WidgetTextField> m_step_value;
	std::shared_ptr<WidgetTextField> m_hr_max_widget;
	std::shared_ptr<WidgetTextField> m_hr_crit_widget;

};

#endif /* FREERUNMENUSAS_H_ */
