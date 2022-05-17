#ifndef MODALWARMUPPARAMS_H_
#define MODALWARMUPPARAMS_H_

#include "Menu.h"

#include "WidgetTextField.h"

/// Окно установки параметров разминки.
class ModalWarmUpParams: public Menu {

public:

	/// Конструктор.
	ModalWarmUpParams();

	/// Деструктор.
	virtual ~ModalWarmUpParams();

private:

	std::shared_ptr<WidgetTextField> m_begin_speed;
	std::shared_ptr<WidgetTextField> m_end_speed;
	std::shared_ptr<WidgetTextField> m_duration;

};

#endif /* MODALWARMUPPARAMS_H_ */
