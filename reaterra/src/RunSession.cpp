#include "RunSession.h"

#include <cmath>
#include <stdexcept>

#include "Context.h"
#include "Logger.h"
#include "Locale.h"

RunSession::RunSession()
: m_started {false}
{
	LD("Ctor");
	init();
	ctx.m_sound.play(SoundInterface::Sound::Welcome);
}

RunSession::~RunSession() {
	LD("Dtor");
}

void RunSession::start(int patient_id, std::string profile, std::string sas) {
	LD("Start session");
	m_sas_mode = sas;
	if (m_started) {
		throw std::runtime_error("Try to start already started session");
	}

	m_started = true;
	init();
	m_patient_id = patient_id;
	m_profile_name = profile;
}

void RunSession::finish() {
	LD("Finish session");

	if (m_started) {
		m_started = false;
		for (int i = 0; !m_sas_mode.empty() && i < 20; ++i) {
			if (ctx.m_treadmill.getSasStepLength() > 127 && ctx.m_treadmill.getSasSymmetry() > 127) {
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (getPatientId() > 0 && getDuration()) {
			ctx.m_db.addRunSession(*this);
		}
	}
	if (m_sas_mode.empty()) {
		init();
	}

}

void RunSession::incSec() {
	if (!m_started) {
		return;
	}

	m_duration++;

	m_hrate = ctx.m_hr_monitor.getHRate();
	m_hrate_sum += m_hrate;
	m_max_hrate = std::max(m_max_hrate, m_hrate);
	m_min_hrate = std::min(m_min_hrate, m_hrate);

	m_max_accel = std::max(m_max_accel, getAcceleration());
	m_max_angle = std::max(m_max_angle, ctx.m_treadmill.getAngle() / 100.f);

	m_speed_sum += ctx.m_treadmill.getActualSpeed() / 10.;

	m_distance += ctx.m_treadmill.getActualSpeed() / 10. / 3600.;

	m_sas_step_length_sum += getSasStepLength();
	m_sas_sym_left_sum += getSasSymmetryLeft();
	m_sas_sym_right_sum += getSasSymmetryRight();

	if (getPatientId() > 0) {
		m_power_mech = getPowerMechByParams(ctx.m_treadmill.getActualSpeed() / 10.,
				ctx.m_treadmill.getAngle() / 100., ctx.m_patient_weight, ctx.m_treadmill.getDirection());

		float tang_tr = tan(Utils::deg2Radian(ctx.m_treadmill.getAngle() / 100.));

		m_energy += Utils::physConvert(Utils::Phys::Energy, Utils::Unit::KWattPerHour, Utils::Unit::Joul, m_power_mech / 1000.0) / 3600.0;

		if(ctx.m_treadmill.getActualSpeed() / 10. < 5.0) {	// Это ходьба
			m_power_aero = ctx.m_treadmill.getActualSpeed() / 10. * (1.675 + 0.3015 * tang_tr) + 3.5;
		} else {										// это бег
			m_power_aero = ctx.m_treadmill.getActualSpeed() / 10. * (3.35 + 0.15075 * tang_tr) + 3.5;
		}

		m_power_mech_sum += m_power_mech;
		m_power_aero_sum += m_power_aero;
	}
}

int RunSession::getDuration() const {
	return m_duration;
}

float RunSession::getDistance() const {
	return m_distance;
}


float RunSession::getEnergy() const {
	return m_energy;
}

float RunSession::getPowerAero() const {
	return m_power_aero;
}

float RunSession::getPowerMech() const {
	return m_power_mech;
}

float RunSession::getAngle() const {
	return ctx.m_treadmill.getAngle() / 100.;
}

float RunSession::getSpeed() const {
	return ctx.m_treadmill.getSpeed() / 10.;
}

using namespace Utils;

std::string RunSession::getDistanceString(float dist) const {
	float value = (dist == -1 ? getDistance() : dist);
	return formatFloat(ctx.m_settings.getParamString("Формат_Расстояние_" + ctx.m_db.getParam("ЕдиницыРасстояние")),
						physConvert(Phys::Distance, Unit::KiloMeter, ctx.m_db.getParamEnum("ЕдиницыРасстояние"), value));
}

std::string RunSession::getAngleString(float angle, std::string format) const {
	float value = (angle == -1 ? getAngle() : angle);
	std::string fmt = (format.empty() ? ctx.m_settings.getParamString("Формат_Угол_" + ctx.m_db.getParam("ЕдиницыУгол")) : format);
	return formatFloat(fmt,	physConvert(Phys::Angle, Unit::Degree, ctx.m_db.getParamEnum("ЕдиницыУгол"), value));
}

std::string RunSession::getSpeedFormat() const {
	return ctx.m_settings.getParamString("Формат_Скорость_" + ctx.m_db.getParam("ЕдиницыСкорость"));
}

std::string RunSession::getAngleFormat() const {
	return ctx.m_settings.getParamString("Формат_Угол_" + ctx.m_db.getParam("ЕдиницыУгол"));
}

std::string RunSession::getSpeedString(float speed, std::string format) const {
	float value = (speed == -1 ? getSpeed() : speed);
	std::string fmt = (format.empty() ? getSpeedFormat() : format);
	return formatFloat(fmt, physConvert(Phys::Speed, Unit::KmPerHour, ctx.m_db.getParamEnum("ЕдиницыСкорость"), value));
}

std::string RunSession::getEnergyString(float energy) const {
	float value = (energy == -1 ? getEnergy() : energy);
	return formatFloat(ctx.m_settings.getParamString("Формат_Энергия_" + ctx.m_db.getParam("ЕдиницыЭнергия")),
						physConvert(Phys::Energy, Unit::Joul, ctx.m_db.getParamEnum("ЕдиницыЭнергия"), value));
}

std::string RunSession::getPowerAeroString(float power) const {
	float value = (power == -1 ? getPowerAero() : power);
	return formatFloat(ctx.m_settings.getParamString("Формат_МощностьАэробная_" + ctx.m_db.getParam("ЕдиницыМощностьАэробная")),
						physConvert(Phys::PowerAir, Unit::MlMinKg, ctx.m_db.getParamEnum("ЕдиницыМощностьАэробная"), value));
}

std::string RunSession::getPowerMechString(float power) const {
	float value = (power == -1 ? getPowerMech() : power);
	return formatFloat(ctx.m_settings.getParamString("Формат_Мощность_" + ctx.m_db.getParam("ЕдиницыМощность")),
						physConvert(Phys::Power, Unit::Watt, ctx.m_db.getParamEnum("ЕдиницыМощность"), value));
}

float RunSession::getSpeedKmH(float value) {
	return Utils::physConvert(Utils::Phys::Speed, ctx.m_db.getParamEnum("ЕдиницыСкорость"), Utils::Unit::KmPerHour, value);
}

float RunSession::getPowerWatt(float value) {
	return Utils::physConvert(Utils::Phys::Power, ctx.m_db.getParamEnum("ЕдиницыМощность"), Utils::Unit::Watt, value);
}

int RunSession::getHrate() const {
	return m_hrate;
}

int RunSession::getPatientId() const {
	return m_patient_id;
}

std::string RunSession::getProfile() const {
	return m_profile_name;
}

float RunSession::getSpeedAvg() const {
	return m_speed_sum / (m_duration ? m_duration : 1);
}

float RunSession::getAccelMax() const {
	return m_max_accel;
}

float RunSession::getAngleMax() const {
	return m_max_angle;
}

int RunSession::getHrateMin() const {
	return m_min_hrate == 999 ? 0 : m_min_hrate;
}

int RunSession::getHrateMax() const {
	return m_max_hrate;
}

float RunSession::getPowerAeroAvg() const {
	return m_power_aero_sum / (m_duration ? m_duration : 1);
}

float RunSession::getPowerMechAvg() const {
	return m_power_mech_sum / (m_duration ? m_duration : 1);
}

float RunSession::getHrateAvg() const {
	return m_hrate_sum / (m_duration ? m_duration : 1);
}

std::string RunSession::getAccelString(float accel) const {
	float value = (accel == -1 ? getAcceleration() : accel);
	return formatFloat(ctx.m_settings.getParamString("Формат_Ускорение_м/с2"), value);
}

float RunSession::getAcceleration() const {
	return Utils::physConvert(Utils::Phys::Speed, Utils::Unit::KmPerHour, Utils::Unit::MPerSec, ctx.m_treadmill.getMaxForwardSpeed()) / ctx.m_treadmill.getAcceleration();
}

void RunSession::init() {
	m_duration = 0;
	m_distance = 0;
	m_energy = 0;
	m_power_mech = 0;
	m_power_aero = 0;
	m_max_accel = 0;
	m_max_angle = 0;
	m_hrate = 0;
	m_min_hrate = 999;
	m_max_hrate = 0;
	m_speed_sum = 0;
	m_power_mech_sum = 0;
	m_power_aero_sum = 0;
	m_hrate_sum = 0;
	m_sas_step_length_sum = 0;
	m_sas_sym_left_sum = 0;
	m_sas_sym_right_sum = 0;
}

float RunSession::getAngleDegree(const float value) {
	return Utils::physConvert(Utils::Phys::Angle, ctx.m_db.getParamEnum("ЕдиницыУгол"), Utils::Unit::Degree, value);
}

std::string RunSession::getPowerMechFormat() const {
	return ctx.m_settings.getParamString("Формат_Мощность_" + ctx.m_db.getParam("ЕдиницыМощность"));
}

float RunSession::getPowerMechByParams(float speed, float angle, float weight, TreadmillDriver::Direction direction) {
	float power = 0;

	float tang_tr = tan(Utils::deg2Radian(angle)) * 100.0;

//	if (direction == TreadmillDriver::Direction::Backward) {
//		if (speed >= 4) {
//			power = (speed * weight * (2.11 - tang_tr * 0.25) + 2.2 * weight - 151) / 10.5;
//		} else {
//			power = (speed * weight * (2.05 - tang_tr * 0.295) - 0.6 * weight - 151) / 10.5;
//		}
//	} else {
//		if (speed >= 4) {
//			power = (speed * weight * (2.11 + tang_tr * 0.25) + 2.2 * weight - 151) / 10.5;
//		} else {
//			power = (speed * weight * (2.05 + tang_tr * 0.295) - 0.6 * weight - 151) / 10.5;
//		}
//	}

	if (direction == TreadmillDriver::Direction::Backward) {
		power = (speed * weight * (2.08 - tang_tr * 0.27) + 0.8 * weight - 151) / 10.5;
	} else {
		power = (speed * weight * (2.08 + tang_tr * 0.27) + 0.8 * weight - 151) / 10.5;
	}



	if (power < 0) {
		power = 0;
	}

	return power;
}

int RunSession::getSasSymmetryLeft() const {
	return !m_sas_mode.empty() && (m_sas_mode != "Прер. ходьба") && getDuration() > 2 ? ctx.m_treadmill.getSasSymmetry() & 0x7F : 0;
}

int RunSession::getSasSymmetryRight() const {
	return !m_sas_mode.empty() && (m_sas_mode != "Прер. ходьба") && getSasSymmetryLeft()  && getDuration() > 2 ? 100 - getSasSymmetryLeft() : 0;
}

int RunSession::getSasSymmetryLeftAvg() const {
	return !m_sas_mode.empty() && (m_sas_mode != "Прер. ходьба") ? (ctx.m_treadmill.getSasSymmetry() & 0x80 ? getSasSymmetryLeft() : m_sas_sym_left_sum / (m_duration ? m_duration : 1)) : 0;
}

int RunSession::getSasSymmetryRightAvg() const {
	return !m_sas_mode.empty() && (m_sas_mode != "Прер. ходьба") && getSasSymmetryLeftAvg() ? 100 - getSasSymmetryLeftAvg() : 0;
}

int RunSession::getSasStepLength() const {
	return !m_sas_mode.empty() && getDuration() > 2 ? ctx.m_treadmill.getSasStepLength() & 0x7F : 0;
}

int RunSession::getSasStepLengthAvg() const {
	return !m_sas_mode.empty() && (m_sas_mode != "Прер. ходьба") ? (ctx.m_treadmill.getSasStepLength() & 0x80 ? getSasStepLength() : m_sas_step_length_sum / (m_duration ? m_duration : 1)) : 0;
}

bool RunSession::isStarted() const {
	return m_started;
}

std::string RunSession::getSasMode() {
	return M(m_sas_mode.empty() ? "Без АРС" : m_sas_mode);
}
