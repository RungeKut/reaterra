#ifndef REHABBOSHRPOWERMENU_H_
#define REHABBOSHRPOWERMENU_H_

#include "RehabBosBaseMenu.h"

/// Меню "бег с БОС задание по ЧСС макс/мощности".
class RehabBosHrPowerMenu: public RehabBosBaseMenu {

public:

	/// Конструктор.
	RehabBosHrPowerMenu();

	/// Деструктор.
	virtual ~RehabBosHrPowerMenu();


private:

	WidgetTextFieldSP m_power_text;

	void onStateChange(Widget::State state) override;
	int getBosCorrectionDelta() override;
	BosCorrectionData getBosCorrectionData(int delta) override;
	void onFieldChange(std::string field, float value) override;

};

#endif /* REHABBOSHRPOWERMENU_H_ */
