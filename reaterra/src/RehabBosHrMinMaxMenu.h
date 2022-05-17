#ifndef REHABBOSHRMINMAXMENU_H_
#define REHABBOSHRMINMAXMENU_H_

#include "RehabBosBaseMenu.h"

/// Меню "бег с БОС задание ЧСС мин/макс".
class RehabBosHrMinMaxMenu: public RehabBosBaseMenu {

public:

	/// Конструктор.
	RehabBosHrMinMaxMenu();

	/// Деструктор.
	virtual ~RehabBosHrMinMaxMenu();


private:

	WidgetImageButtonSP m_bell_btn;
	WidgetTextFieldSP m_max_hr_text;
	WidgetTextFieldSP m_min_hr_text;

	bool m_bell;
	int m_hr_alarm_cnt;
	std::vector<std::string> m_bos_params_low;
	std::vector<std::string> m_bos_params_high;
	void onMaxPossibleHrChange(int value) override;
	void onStateChange(Widget::State state) override;
	int getBosCorrectionDelta() override;
	BosCorrectionData getBosCorrectionData(int delta) override;
	void onTick() override;

};

#endif /* REHABBOSHRMINMAXMENU_H_ */
