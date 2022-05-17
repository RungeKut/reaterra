#ifndef DISPLAYLIGHTNESSPWM_H_
#define DISPLAYLIGHTNESSPWM_H_

#include <chrono>

#include "ThreadClass.h"

class DisplayLightnessPwm: public ThreadClass {
public:
	DisplayLightnessPwm();
	virtual ~DisplayLightnessPwm();

	bool process() override;
	bool init() override;

private:
	int m_period_micros;
	int m_up_time_micros;
	int m_fd_sysfs;
	int m_duty;

};

#endif /* DISPLAYLIGHTNESSPWM_H_ */
