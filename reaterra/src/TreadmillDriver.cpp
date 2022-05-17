#include "TreadmillDriver.h"

#include <chrono>
#include "modbus-private.h"

#include "Context.h"
#include "EnumExtension.h"

namespace treadmill {
	void logger(char level, char *msg) {
		ctx.m_treadmill.log(level, std::string(msg));
	}
}

//TODO: При установке регистров с предварительным чтением проверять что действительно надо записывать новое значение

TreadmillDriver::TreadmillDriver()
:	m_modbus {nullptr},
	m_timer {[this](int id) {

				if (!m_interatrion) {
					return;
				}

				if (m_connected) {
					if (!m_poll_cnt--) {
						m_poll_cnt = m_poll_cnt_max - 1;
						m_queue.putEvent(Command{WRITE, Reg::PLC_ID, {150}});
					}

					if (!m_ready) {
						m_queue.putEvent(Command{READ, Reg::PLC_STAT, {}, [this](std::vector<unsigned short> v) {
								m_registers[Reg::PLC_STAT] = v.at(0);
								m_ready = v.at(0) & 0x0001;
								if (m_ready) {
									ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::TreadmillReady));
								}
								//TODO: Сброс ошибок ??  PLC_CS.DAL  = 1
							}
						});
					} else {


						m_queue.putEvent(Command{READ, Reg::PLC_STAT, {}, [this](std::vector<unsigned short> v) {

							m_registers[Reg::PLC_STAT] = v.at(to_base(Reg::PLC_STAT) - 1);
							bool refresh_flag {false};

							bool r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x0001; //BI
							if (r != m_ready) {
								if (r) {
									LI("Treadmill is ready");
								} else {
									LI("Treadmill is NOT ready");
								}
								refresh_flag = true;
							}
							m_ready = r;

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x0002; //BC
							if (r != m_calibration) {
								if (r) {
									LI("Treadmill is calibrating");
								} else {
									LI("Treadmill is in working mode");
								}
								refresh_flag = true;
							}
							m_calibration = r;

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x0004; //BTA
							if (r != m_set_point) {
								if (r) {
									LI("Treadmill is setting point");
								} else {
									LI("Treadmill completed set point");
								}
								refresh_flag = true;
							}
							m_set_point = r;

							if (getControllerVersion() > 1) {
								r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x08; //
								if (r != m_pfreq_error) {
									if (r) {
										LI("Frequency converter error");
									} else {
										LI("Frequency converter error cleared");
									}
									refresh_flag = true;
								}
								m_pfreq_error = r;

								r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x10; //
								if (r != m_actuator_overload) {
									if (r) {
										LI("Actuator overload error");
									} else {
										LI("Actuator overload error cleared");
									}
									refresh_flag = true;
								}
								m_actuator_overload = r;

								r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x1000; //
								if (r != m_drive_overheat_error) {
									if (r) {
										LI("Drive overheat error");
									} else {
										LI("Drive overheat error cleared");
									}
									refresh_flag = true;
								}
								m_drive_overheat_error = r;

								r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x2000; //
								if (r != m_fall_happen) {
									if (r) {
										LI("Fall happen error");
									} else {
										LI("Fall happen error cleared");
									}
									refresh_flag = true;
								}
								m_fall_happen = r;

								r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x4000; //
								if (r != m_optic_error) {
									if (r) {
										LI("Optic error");
									} else {
										LI("Optic error cleared");
									}
									refresh_flag = true;
								}
								m_optic_error = r;
							}

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x20; //EK
							if (r != m_emegency_stop) {
								if (r) {
									LI("Emergency button pressed");
								} else {
									LI("Emergency button released");
								}
								refresh_flag = true;
							}
							m_emegency_stop = r;

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x40; //OS
							if (r != m_cross_error) {
								if (r) {
									LI("Overstep happened");
								} else {
									LI("Overstep cleared");
								}
								refresh_flag = true;
							}
							m_cross_error = r;

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x80; // INV
							if (r != m_pfreq_connected) {
								if (r) {
									LI("Freq converter disconnected");
								} else {
									LI("Freq converter connected");
								}
								refresh_flag = true;
							}
							m_pfreq_connected = r;

							r = v.at(to_base(Reg::PLC_STAT) - 1) & 0x200; // AM
							if (r != m_fall_on_tread) {
								if (r) {
									LI("Fall on the treadmill");
								} else {
									LI("Fall on the treadmill cleared");
								}
								refresh_flag = true;
							}
							m_fall_on_tread = r;


							if (refresh_flag) {
								ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
							}


							m_registers[Reg::PLC_AX] = v.at(to_base(Reg::PLC_AX) - 1);
							m_angle = v.at(to_base(Reg::PLC_AX) - 1);


							m_registers[Reg::PLC_ACX] = v.at(to_base(Reg::PLC_ACX) - 1);
							m_calib_angle_x = static_cast<short>(v.at(to_base(Reg::PLC_ACX) - 1));


							m_registers[Reg::PLC_ACY_V0] = v.at(to_base(Reg::PLC_ACY_V0) - 1);
							m_calib_angle_y = static_cast<short>(v.at(to_base(Reg::PLC_ACY_V0) - 1));


							if (m_sas_exists) {
								m_registers[Reg::PLC_CS] = v.at(to_base(Reg::PLC_CS) - 1);
								r = v.at(to_base(Reg::PLC_CS) - 1) & 0x2000; //SAS connection OK
//								if (r && m_sas_conn_error) {
//									LI("SAS connection error cleared");
//									m_sas_conn_error = false;
//								}
								if (!r && !m_sas_conn_error) {
									LI("SAS connection error");
									m_sas_conn_error = true;
								}
							}

							m_registers[Reg::PLC_CD] = v.at(to_base(Reg::PLC_CD) - 1);


							RunState state_cur;
							if (v.at(to_base(Reg::PLC_CD) - 1) & 0x8000) {
								state_cur = RunState::Run;
							} else if (v.at(to_base(Reg::PLC_CD) - 1) & 0x4000) {
								state_cur = RunState::Stop;
							} else {
								state_cur = RunState::Pause;
							}

//							LD("Read state: " + std::to_string(to_base(state_cur)));
							if (m_state_update) {
								m_state = state_cur;
								m_speed = v.at(to_base(Reg::PLC_CD) - 1) & 0x3FF;
//								LD("Update state: " + std::to_string(to_base(state_cur)));
							} else {
								LD("m_state_update == false");
								if (state_cur == m_state) {
//									LD("m_state_update = true");
									m_state_update = true;
								}
							}

//							if (m_state_update) {
//								if (v.at(to_base(Reg::PLC_CD) - 1) & 0x8000) {
//									m_state = RunState::Run;
//								} else if (v.at(to_base(Reg::PLC_CD) - 1) & 0x4000) {
//									m_state = RunState::Stop;
//								} else {
//									m_state = RunState::Pause;
//								}
//								LD("Update state: " + std::to_string(to_base(m_state)));
//								m_speed = v.at(to_base(Reg::PLC_CD) - 1) & 0x3FF;
//							}

							if (v.at(to_base(Reg::PLC_CD) - 1) & 0x1800) {
								m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this](std::vector<unsigned short> v) {
									    LD("Try to set PLC_CL to " + std::to_string(static_cast<unsigned short>(v.at(0) & ~0xC000)));
									    //TODO: Выяснить необходимость установки PLC_CL от PLC_STAT
										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>(v.at(0) & ~0xC000)}});
									}
								});
							}


							m_registers[Reg::PLC_CTA] = v.at(to_base(Reg::PLC_CTA) - 1);

							m_registers[Reg::PLC_CTD] = v.at(to_base(Reg::PLC_CTD) - 1);

							m_registers[Reg::PLC_SD] = v.at(to_base(Reg::PLC_SD) - 1);
							m_actual_speed = 0x3FF & v.at(to_base(Reg::PLC_SD) - 1);

							m_registers[Reg::PLC_ED] = v.at(to_base(Reg::PLC_ED) - 1);

							m_registers[Reg::PLC_CL] = v.at(to_base(Reg::PLC_CL) - 1);

						}, 11
						});

						if (m_sas_exists) {

							if (m_read_sas_service_regs) {
								m_queue.putEvent(Command{READ, Reg::PLC_ARS1, {}, [this](std::vector<unsigned short> v) {
									m_registers[Reg::PLC_ARS1] = v.at(0);
									m_registers[Reg::PLC_ARS2] = v.at(1);
									m_registers[Reg::PLC_ARS3] = v.at(2);
									m_registers[Reg::PLC_ARS4] = v.at(3);
								}, 4
								});

							} else {

//							LD("READ SAS");
							m_queue.putEvent(Command{READ, Reg::PLC_SAS_STEP, {}, [this](std::vector<unsigned short> v) {

								m_registers[Reg::PLC_SAS_STEP] = v.at(0);
								m_registers[Reg::PLC_SAS_MODE] = v.at(to_base(Reg::PLC_SAS_MODE) - to_base(Reg::PLC_SAS_STEP));
								m_registers[Reg::PLC_SAS_SYM_LEFT] = v.at(to_base(Reg::PLC_SAS_SYM_LEFT) - to_base(Reg::PLC_SAS_STEP));
								m_registers[Reg::PLC_SAS_STATUS] = v.at(to_base(Reg::PLC_SAS_STATUS) - to_base(Reg::PLC_SAS_STEP));

//								LD("PLC_SAS_STATUS: " + std::to_string(m_registers[Reg::PLC_SAS_STATUS]));

								if (!m_sas_test_result && (m_registers.at(Reg::PLC_SAS_STATUS) & 0xE000)) {
									m_sas_test_result = m_registers.at(Reg::PLC_SAS_STATUS) & 0xE000;
								} else {
									m_registers[Reg::PLC_SAS_STATUS] = (m_registers.at(Reg::PLC_SAS_STATUS) & ~0xE000) | m_sas_test_result;
								}

								if (!m_sas_test_complete && !isSasTestNeeded()) {
									m_sas_test_complete = true;
									ctx.m_run_sas_test = false;
									setState(RunState::Stop);
									ctx.m_treadmill.setSpeed(10);
									m_sas_accel_restore = true;
									ctx.m_treadmill.doZeroPoint();
								}

								m_sas_step_length = v.at(0);  // PLC_SAS_STEP 13
								m_sas_symmetry = v.at(1);   // PLC_SAS_SYM_LEFT 14

								bool r = v.at(4) & 0x2;  // PLC_SAS_STATUS 17
								if (r != m_is_feet_ready) {
									if (r) {
										LI("SAS feet is ready");
									} else {
										LI("SAS feet is ready cleared");
									}
									ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
								}
								m_is_feet_ready = r;

								r = v.at(4) & 0x1;  // PLC_SAS_STATUS 17
								if (r != m_is_man_on_tread) {
									if (r) {
										LI("SAS man is on tread");
									} else {
										LI("SAS man is off tread");
									}
									ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
								}
								m_is_man_on_tread = r;

							}, 6
							});

							}
						}

						if (m_unl_exists) {
							m_queue.putEvent(Command{READ, Reg::PLC_UNL_BATT, {}, [this](std::vector<unsigned short> v) {

								m_registers[Reg::PLC_UNL_BATT] = v.at(0);

								m_unl_batt_warn = v.at(0) & 0x8000;
								m_unl_batt_level = v.at(0) & 0xFF;

							}, 1
							});
						}

						if (!m_zero_point) {
							checkZeroPoint();
						}

//						m_first_run = false;
					}
				}
			}
		},
	m_queue {"Modbus"},
	m_poll_cnt {0},
	m_connected {false},
	m_direction {Direction::Forward},
	m_angle {0},
	m_acceleration {0},
	m_deceleration {0},
	m_ready {false},
	m_state {RunState::Stop},
	m_cmd_in_progress {false},
	m_emegency_stop {false},
	m_cross_error {false},
	m_plc_connected {true},
	m_pfreq_connected {false},
	m_fall_on_tread {false},
	m_calibration {false},
	m_calib_angle_x {0},
	m_calib_angle_y {0},
	m_actual_speed {0},
	m_zero_point {false},
	m_state_update {true},
	m_set_point {false},
	m_remote {false},
	m_paused_angle {0},
	m_paused_speed {0},
	m_paused {false},
	m_actuator_overload {false},
	m_pfreq_error {false},
	m_drive_overheat_error {false},
	m_fall_happen {false},
	m_optic_error {false},
	m_sas_conn_error {false},
	m_is_feet_ready {false},
	m_is_man_on_tread {false},
	m_sas_exists {false},
	m_sas_mode {0},
	m_treadmill_type {100},
	m_controller_config {0},
//	m_first_run {true},
	m_interatrion {true},
	m_sas_symmetry {50},
	m_sas_step_length {0},
	m_unl_batt_level {100},
	m_unl_batt_warn {false},
	m_unl_exists {false},
	m_init_completed {false},
	m_init_waiting {true}
{
	LD("Ctor");
	m_speed = ctx.m_settings.getParamInt("speed_min");

	if (m_speed == 0) {
		m_speed = 10;
	}

	m_registers[Reg::PLC_SAS_STATUS] = 0;

	m_modbus_attempt_max = ctx.m_settings.getParamInt("modbus_attempts");
	m_queue_limit =  ctx.m_settings.getParamInt("modbus_queue_limit");
	int poll_time = ctx.m_settings.getParamInt("treadmill_poll_time");
	m_poll_cnt_max = HEART_BEAT_TIMEOUT / poll_time;

	m_controller_version = ctx.m_settings.getParamInt("controller_version");

	start();
	m_timer.startTimerMillis(poll_time);
}

TreadmillDriver::~TreadmillDriver() {
	stop();

	if (m_modbus) {
		modbus_close(m_modbus);
		modbus_free(m_modbus);
	}
	LD("Dtor");
}

bool TreadmillDriver::process() {
	bool ret {true};

	Command cmd {m_queue.getEvent()};

	int queue_size = m_queue.getSize();
	if (queue_size >= m_queue_limit) {
		LE("Modbus queue is overloaded: " + std::to_string(queue_size));
	}

	int i;
	for (i = 0; i < m_modbus_attempt_max; ++i) {
		if (cmd.read) {
			unsigned short buff[12];
			if (modbus_read_registers(m_modbus, static_cast<int>(cmd.regnum) + ctx.m_settings.getParamInt("modbus_addr_shift"), cmd.qnt, buff) != MODBUS_FAIL) {
				cmd.data.insert(cmd.data.begin(), buff, buff + cmd.qnt);
				cmd.on_read(cmd.data);
				break;
			}
		} else {
			if (modbus_write_register(m_modbus, static_cast<int>(cmd.regnum) + ctx.m_settings.getParamInt("modbus_addr_shift"), cmd.data.data()[0]) != MODBUS_FAIL) {
//				LD("WR: " +  getRegisterName(cmd.regnum) + "  " + std::to_string(cmd.data.data()[0]));
				break;
			}
		}
	}

	bool prev_state = m_plc_connected;
	if (i == m_modbus_attempt_max) {
		LE("Failed to " + (cmd.read? "read " : "write ") + getRegisterName(cmd.regnum) + " register: " + std::string(modbus_strerror(errno)));
		ret = false;
		m_plc_connected = false;
	} else {
		m_plc_connected = true;
	}
	if (prev_state != m_plc_connected) {
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	int delay = ctx.m_settings.getParamInt("modbus_send_delay");
	if (delay) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}

	return ret;
}

bool TreadmillDriver::init() {

	if (ctx.m_settings.getParamInt("check_connection") == 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		return false;
	}

	m_queue.clear();
	if (m_connected) {
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::TreadmillDisconnected));
	}
	if (m_modbus) {
		modbus_close(m_modbus);
		modbus_free(m_modbus);
		m_modbus = nullptr;
	}
	m_connected = false;
	m_ready = false;
	m_modbus = modbus_new_rtu(ctx.m_settings.getParamString("treadmill_port_name").c_str(),
			ctx.m_settings.getParamInt("treadmill_port_speed"), 'N', 8, 1, treadmill::logger);
	if(!m_modbus)	{
		LE("Failed to create a modbus context: " + std::string(modbus_strerror(errno)));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		return false;
	}

	if (ctx.m_settings.getParamInt("modbus_debug")) {
		m_modbus->debug = 1;
	}

//	modbus_enable_rpi(mb_ctx, 1);
//	modbus_configure_rpi_bcm_pin(mb_ctx, 4); // configures BCM pin # 18 for switching
//	modbus_rpi_pin_export_direction(mb_ctx); // exports configured BCM pin #

	//Попытка установки соединения через инициализированный контекст
	if (modbus_connect(m_modbus) != 0)
	{
		LE("Connection failed: " + std::string(modbus_strerror(errno)));
		modbus_close(m_modbus);
		modbus_free(m_modbus);
		m_modbus = nullptr;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		return false;
	}


	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (modbus_set_slave(m_modbus, ctx.m_settings.getParamInt("treadmill_modbus_address")) == MODBUS_FAIL) {
		LE("Failed to set slave device: " + std::string(modbus_strerror(errno)));
		return false;
	}

	LI("Modbus port is opened");
	m_connected = true;
	ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::TreadmillConnected));

	initTreadmill();

	return true;
}

void TreadmillDriver::log(char level, std::string msg) {
	msg = "Modbus >> " + msg;
	if (level == 'D') {
		LD(msg);
	} else if (level == 'I') {
		LI(msg);
	} else if (level == 'E') {
		LE(msg);
	}
}

bool TreadmillDriver::isConnected() const {
	return m_connected;
}

std::string TreadmillDriver::getRegisterName(Reg reg) const {
	switch (reg) {
	case Reg::PLC_ID:
		return "REG_PLC_ID";
	case Reg::PLC_ACX:
		return "REG_PLC_ACX";
	case Reg::PLC_AX:
		return "REG_PLC_AX";
	case Reg::PLC_CAL:
		if (ctx.m_treadmill.getControllerVersion() == 0) {
			return "REG_PLC_ACY";
		}
		return "REG_PLC_CAL";
	case Reg::PLC_CD:
		return "REG_PLC_CD";
	case Reg::PLC_CL:
		return "REG_PLC_CL";
	case Reg::PLC_CS:
		return "REG_PLC_CS";
	case Reg::PLC_CTA:
		return "REG_PLC_CTA";
	case Reg::PLC_CTD:
		return "REG_PLC_CTD";
	case Reg::PLC_ED:
		return "REG_PLC_ED";
	case Reg::PLC_SD:
		return "REG_PLC_SD";
	case Reg::PLC_STAT:
		return "REG_PLC_STAT";
	case Reg::PLC_EX:
		return "REG_PLC_EX";
	case Reg::PLC_EX_V0:
		return "REG_PLC_EX_V0";
	case Reg::PLC_SAS_MODE:
		return "REG_PLC_SAS_MODE";
	case Reg::PLC_SAS_STEP:
		return "REG_PLC_SAS_STEP";
	case Reg::PLC_SAS_STATUS:
		return "REG_PLC_SAS_STATUS";
	case Reg::PLC_SAS_SYM_LEFT:
		return "REG_PLC_SAS_SYM_LEFT";
	case Reg::PLC_VER:
		return "REG_PLC_VER";
	case Reg::PLC_CONF:
		return "REG_PLC_CONF";
	case Reg::PLC_UNL_BATT:
		return "REG_UNL_BATT";
	case Reg::PLC_TYPE:
		if (ctx.m_treadmill.getControllerVersion() == 0) {
			return "REG_PLC_CAL_V0";
		}
		return "REG_PLC_TYPE";
	}
	return "";
}

bool TreadmillDriver::isReady() const {
	return m_ready;
}

TreadmillDriver::RunState TreadmillDriver::getState() const {
//	LD("Get state: " + std::to_string(to_base(m_state)));
	return m_state;
}

void TreadmillDriver::setState(RunState state, bool set_cl) {
	LD("Set state: " + std::to_string(to_base(state)));
	m_state = state;
	m_state_update = false;
	m_queue.putEvent(Command{READ, Reg::PLC_CD, {}, [this, state, set_cl](std::vector<unsigned short> v) {
			if (state == RunState::Pause) {
				LD("Change state to RunState::Pause");
				m_paused = true;
				if (!m_remote) {
					m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this](std::vector<unsigned short> v) {
							m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>(v.at(0) | 0x2000)}});
							m_state_update = true;
						}
					});
				}

				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>(v.at(0) & ~0xC000)}});
			} else if (state == RunState::Run) {
//				LD("Change state == RunState::Run");
//				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>((v.at(0) & 0x3FFF) | 0x8000)}});

//				if (!m_remote) {
//					LD("!m_remote");
					if (m_paused) {
//						LD("m_paused");
//						setAngle(m_paused_angle);
//						setSpeed(m_paused_speed);
//						setAcceleration(getAcceleration());
//						setAngle(m_paused_angle);
						LD("Set angle to " + std::to_string(m_paused_angle) + "  speed to " + std::to_string(m_paused_speed));
					}
//					m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this](std::vector<unsigned short> v) {
//							m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((v.at(0) & 0x1FFF) | 0x1000)}});
//						}
//					});
//				}

					//Установка бита 12 AD(выход на уставку) регистра PLC_CL
					if (set_cl) {
						m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this](std::vector<unsigned short> v) {

								m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>(v.at(0) | 0x1000)}});
							}
						});
					}

				m_paused = false;
				LD("Change state to RunState::Run");
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>((v.at(0) & 0x3FFF) | 0x8000)}});
				m_state_update = true;
//				setAngle(m_paused_angle);
//				setSpeed(m_paused_speed);
//				setAcceleration(getAcceleration());
			} else if (state == RunState::Stop) {
				if (!m_remote) {
					setAngle(0);
				}
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>((v.at(0) & ~0x8000) | 0x4000)}});
				m_state_update = true;
				m_paused = false;
				LD("Change state to RunState::Stop");
//				ctx.m_session.finish();
			}
//			m_state_update = true;
		}
	});
}

void TreadmillDriver::setLift(Lift lift) {
	m_queue.putEvent(
			Command{READ, Reg::PLC_CD, {}, [this, lift](std::vector<unsigned short> v) {
						if (!(v.at(0) & 0x1800)) {
							m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this, lift](std::vector<unsigned short> v) {
									if (lift == Lift::Stop) {
										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>(v.at(0) & ~0xC000)}});
									} else if (lift == Lift::Up) {
										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((v.at(0) & ~0x4000) | 0x8000)}});
									} else if (lift == Lift::Down) {
										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((v.at(0) & ~0x8000) | 0x4000)}});
									}
								}
							});
						} else {
							LT("Pult is working...");
							m_queue.putAsNext(Command{READ, Reg::PLC_CL, {}, [this, lift](std::vector<unsigned short> v) {
									if (lift == Lift::Stop) {
										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>(v.at(0) & ~0xC000)}});
//									} else if (lift == Lift::Up) {
//										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((v.at(0) & ~0x4000) | 0x8000)}});
//									} else if (lift == Lift::Down) {
//										m_queue.putAsNext(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((v.at(0) & ~0x8000) | 0x4000)}});
									}
								}
							});
						}
					}
				});
}

void TreadmillDriver::setRegister(Reg reg, int value) {
	m_queue.putEvent(Command{WRITE, reg, {static_cast<unsigned short>(value)}});
}

int TreadmillDriver::getRegister(Reg reg) {
	int value {-1};
	std::mutex m;
	std::unique_lock<std::mutex> lock(m);
	std::condition_variable cv;
	m_queue.putEvent(Command{READ, reg, {}, [this, &cv, &value](std::vector<unsigned short> v) {
			value = v.at(0);
			cv.notify_all();
		}
	});
	while (value < 0) {
		cv.wait(lock);
	}
	return value;
}

void TreadmillDriver::setRemoteControlLifting(bool enable) {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this, enable](std::vector<unsigned short> v) {
			m_remote = enable;
			if (enable) {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x0001)}});
			} else {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) & ~0x0001)}});
			}
		}
	});
}

void TreadmillDriver::resetError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x0002)}});
			m_cross_error = false;
		}
	});
}

int TreadmillDriver::getAccelValue(const int id) {
	return Utils::kmh2ms(getMaxForwardSpeed() / 10.) / getAccelTimeValue(id) * 1000;
}

int TreadmillDriver::getAccelTimeValue(const int id) {
	int times[] = {131, 66, 33, 16, 8, 5, 3};
	return times[id - 1];
}

bool TreadmillDriver::isPlcConnected() {
	return m_plc_connected;
}

bool TreadmillDriver::isPFreqAlarm() {
	return m_pfreq_connected;
}

bool TreadmillDriver::isFallenOnTread() {
	return m_fall_on_tread;
}

bool TreadmillDriver::isCalibrating() {
	return m_calibration;
}

void TreadmillDriver::toggleMode() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x0004)}});
		}
	});
}

int TreadmillDriver::getCalibrationAngleX() {
	return m_calib_angle_x;
}

//int TreadmillDriver::getCalibrationAngleY() {
//	return m_calib_angle_y;
//}

uint16_t TreadmillDriver::getRegisterState(Reg name) {
	if (m_registers.find(name) != m_registers.end()) {
		return m_registers.at(name);
	} else {
		return 0;
	}
}

int TreadmillDriver::getActualSpeed() const {
	return m_actual_speed;
}

bool TreadmillDriver::isZeroPoint() const {
	return m_zero_point;
}

void TreadmillDriver::doZeroPoint() {
	if (getState() != TreadmillDriver::RunState::Stop) {
		setState(TreadmillDriver::RunState::Stop);
	}

	if (getAngle() > 10) {
		setAngle(0);
	}
	checkZeroPoint();
}

void TreadmillDriver::initTreadmill() {

	m_init_waiting = true;
	LD("INIT STARTED");

	m_queue.putEvent(Command{READ, Reg::PLC_VER, {}, [this](std::vector<unsigned short> v) {
			m_registers[Reg::PLC_VER] = v.at(0);
			int treadmill_type = ctx.m_settings.getParamInt("treadmill_type");
			m_controller_version = v.at(0);
			if (getControllerVersion() > 0) {
				if (treadmill_type == 0) {
					m_queue.putEvent(Command{READ, Reg::PLC_TYPE, {}, [this](std::vector<unsigned short> v) {
							m_registers[Reg::PLC_TYPE] = v.at(0);
							m_treadmill_type = v.at(0) & 0xFFF;
//							m_sas_exists = v.at(0) & 0x8000;
						}
					});
				}
				m_queue.putEvent(Command{READ, Reg::PLC_CONF, {}, [this](std::vector<unsigned short> v) {
						m_registers[Reg::PLC_CONF] = v.at(0);
					    m_controller_config = v.at(0);
						m_sas_exists = v.at(0) & 0x1;
						m_unl_exists = v.at(0) & 0x2;
						LD("CONFIG IS SET 1");
						m_init_waiting = false;
					}
				});
			} else if (treadmill_type > 0) {
				m_treadmill_type = treadmill_type;
				LD("CONFIG IS SET 2");
				m_init_waiting = false;
			}
		}
	});

//	for (int i = 0; i < 50 && m_init_waiting; ++i) {
//		LD("WAIT FOR INIT");
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}

	LD("CONTINUE");
	// Отключается ручной пульт
	setRemoteControlLifting(false);

	ctx.m_treadmill.setSpeed(10);
	setDirection(TreadmillDriver::Direction::Forward);
	doZeroPoint();


	setAcceleration(getAccelTimeValue(ctx.m_accel) * 10);

	setDeceleration(getAccelTimeValue(ctx.m_decel) * 10);
}

int TreadmillDriver::getAcceleration() const {
	return m_acceleration;
}

void TreadmillDriver::setAcceleration(int acceleration) {
	m_acceleration = acceleration;
	m_queue.putEvent(Command{WRITE, Reg::PLC_CTA, {static_cast<unsigned short>(acceleration)}});
}

int TreadmillDriver::getAngle() const {
	return m_angle;
}

void TreadmillDriver::setAngle(int angle, bool not_set) {

	m_paused_angle = angle;
	if (not_set) {
		return;
	}

	m_angle = angle;
	LT("Set angle: " + std::to_string(angle));
	m_queue.putEvent(Command{WRITE, Reg::PLC_CL, {static_cast<unsigned short>((angle & 0xFFF) | 0x1000)}});
	if (!angle) {
		m_set_point = 1;
	}
}

int TreadmillDriver::getDeceleration() const {
	return m_deceleration;
}

void TreadmillDriver::setDeceleration(int deceleration) {
	m_deceleration = deceleration;
	m_queue.putEvent(Command{WRITE, Reg::PLC_CTD, {static_cast<unsigned short>(deceleration)}});
}

TreadmillDriver::Direction TreadmillDriver::getDirection() const {
	return m_direction;
}

void TreadmillDriver::setDirection(Direction direction) {
	if (m_direction == direction/* || m_cmd_in_progress*/) {
		return;
	}
	m_direction = direction;

	m_queue.putEvent(Command{READ, Reg::PLC_CD, {}, [this, direction](std::vector<unsigned short> v) {
			if (direction == Direction::Forward) {
				LD("SET DIRECTION FORWARD");
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>(v.at(0) & ~0x2000)}});
			} else {
				LD("SET DIRECTION BACKWARD");
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>(v.at(0) | 0x2000)}});
			}
		}
	});
}

int TreadmillDriver::getSpeed() const {
	return m_speed;
}

void TreadmillDriver::setSpeed(int speed) {
	m_speed = speed;
	m_paused_speed = speed;
	LD("Speed set to " + std::to_string(speed));
	m_state_update = false;
	m_queue.putEvent(Command{READ, Reg::PLC_CD, {}, [this, speed](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>((v.at(0) & ~0x03ff) | (speed & 0x03ff))}});
			m_state_update = true;
		}
	});
}


bool TreadmillDriver::isCrossError() const {
	return m_cross_error;
}

bool TreadmillDriver::isEmegencyStop() const {
	return m_emegency_stop;
}

int TreadmillDriver::getMaxForwardSpeed() {
	int ret = getTreadmillType();
//	if (ctx.m_settings.getParamString("treadmill_type") == "10") {
//		ret = 100;
//	} else if (ctx.m_settings.getParamString("treadmill_type") == "22") {
//		ret = 220;
//	} else if (ctx.m_settings.getParamString("treadmill_type") == "30") {
//		ret = 300;
//	}

	return ret;
}

int TreadmillDriver::getMaxBackwardSpeed() {
	int ret = getMaxForwardSpeed() / 2;
//	if (ctx.m_settings.getParamString("treadmill_type") == "10") {
//		ret = 50;
//	} else if (ctx.m_settings.getParamString("treadmill_type") == "22") {
//		ret = 110;
//	} else if (ctx.m_settings.getParamString("treadmill_type") == "30") {
//		ret = 150;
//	}
	return ret;
}

int TreadmillDriver::getMinSpeed() {
	int ret = ctx.m_settings.getParamInt("speed_min");

	if (ret == 0) {
		if (getMaxForwardSpeed() >= 220) {
			ret = 10;
		} else {
			ret = 1;
		}
	}

	return ret;
}

int TreadmillDriver::getMaxAngle() {
	return ctx.m_settings.getParamInt("angle_max");
}

int TreadmillDriver::getSpeedStep() {
	int ret = 1;
	if (getMaxForwardSpeed() > 220) {
		ret = 5;
	}
	return ret;
}

void TreadmillDriver::setExternal(ExtMode mode) {
	if (getControllerVersion() == 0) {
		m_queue.putEvent(Command{READ, Reg::PLC_EX_V0, {}, [this, mode](std::vector<unsigned short> v) {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_EX_V0, {static_cast<unsigned short>((v.at(0) & ~0x0003) | to_base(mode))}});
			}
		});
	} else {
		m_queue.putEvent(Command{READ, Reg::PLC_EX, {}, [this, mode](std::vector<unsigned short> v) {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_EX, {static_cast<unsigned short>((v.at(0) & ~0x0003) | to_base(mode))}});
			}
		});
	}
}

void TreadmillDriver::setDNF(bool value) {
	m_queue.putEvent(Command{READ, Reg::PLC_CD, {}, [this, value](std::vector<unsigned short> v) {
			if (value) {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>(v.at(0) | 0x400)}});
			} else {
				m_queue.putAsNext(Command{WRITE, Reg::PLC_CD, {static_cast<unsigned short>(v.at(0) & ~0x400)}});
			}
		}
	});
}

void TreadmillDriver::setSpeedCalib(unsigned short value) {
	if (getControllerVersion() == 0) {
		m_queue.putEvent(Command{WRITE, Reg::PLC_CAL_V0, {value}});
	} else {
		m_queue.putEvent(Command{WRITE, Reg::PLC_CAL, {value}});
	}
}

void TreadmillDriver::actuatorCalibReset() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x0010)}});
		}
	});
}

void TreadmillDriver::actuatorCalibSet() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x0020)}});
		}
	});
}

bool TreadmillDriver::isSettingPoint() {
	return m_set_point;
}

bool TreadmillDriver::isPFreqError() {
	return m_pfreq_error;
}

bool TreadmillDriver::isActuatorOverloaded() {
	return m_actuator_overload;
}

void TreadmillDriver::resetPfError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x40)}});
			m_pfreq_error = false;
		}
	});
}

void TreadmillDriver::resetActuatotError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x08)}});
			m_actuator_overload = false;
		}
	});
}

bool TreadmillDriver::isDriveOverheatError() const {
	return m_drive_overheat_error;
}

bool TreadmillDriver::isFallHappen() const {
	return m_fall_happen;
}

void TreadmillDriver::resetFallenError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x80)}});
			m_fall_happen = false;
		}
	});
}

bool TreadmillDriver::isOpticError() const {
	return m_optic_error;
}

void TreadmillDriver::resetOpticError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x100)}});
			m_optic_error = false;
		}
	});
}

bool TreadmillDriver::isSasConnectionError() const {
	return m_sas_conn_error;
}

bool TreadmillDriver::isFeetReady() const {
	return m_is_feet_ready;
}

void TreadmillDriver::stopInteraction() {
	m_interatrion = false;
}

void TreadmillDriver::startInteraction() {
	m_interatrion = true;
}

int TreadmillDriver::getControllerVersion() {
	return m_controller_version;
}

bool TreadmillDriver::isUnloadingExists() {
	return m_controller_config & 0x2;
}

void TreadmillDriver::resetFallenOnTreadError() {
	m_queue.putEvent(Command{READ, Reg::PLC_CS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_CS, {static_cast<unsigned short>(v.at(0) | 0x200)}});
			m_fall_happen = false;
		}
	});
}

int TreadmillDriver::getUnlBattLevel() const {
	return m_unl_batt_level;
}

bool TreadmillDriver::isUnlBattWarn() const {
	return m_unl_batt_warn;
}

bool TreadmillDriver::isInitCompleted() const {
	return !m_init_waiting;
}

bool TreadmillDriver::isSasManOnTread() const {
	return m_is_man_on_tread;
}

void TreadmillDriver::startSasTest(const bool stat_test) {
	m_queue.putEvent(Command{READ, Reg::PLC_SAS_MODE, {}, [this, stat_test](std::vector<unsigned short> v) {
			unsigned short mask = stat_test ? 0xC000 : 0x8000;
			m_queue.putAsNext(Command{WRITE, Reg::PLC_SAS_MODE, {static_cast<unsigned short>(v.at(0) | mask)}});
			m_registers[Reg::PLC_SAS_MODE] = v.at(0) | mask;
		}
	});
}

void TreadmillDriver::restartSasTest() {
	m_queue.putEvent(Command{READ, Reg::PLC_SAS_STATUS, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_SAS_STATUS, {static_cast<unsigned short>(v.at(0) & ~0xE000)}});
			m_registers[Reg::PLC_SAS_STATUS] = v.at(0) & ~0xE000;

			m_queue.putEvent(Command{READ, Reg::PLC_SAS_MODE, {}, [this](std::vector<unsigned short> v) {
					m_queue.putAsNext(Command{WRITE, Reg::PLC_SAS_MODE, {static_cast<unsigned short>(v.at(0) & ~0x8000)}});
					m_registers[Reg::PLC_SAS_MODE] = v.at(0) & ~0x8000;
					m_sas_test_result = 0;
					m_sas_test_complete = false;
				}
			});
		}
	});

}


std::vector<unsigned short> TreadmillDriver::getSasSensorsStatus() {
	std::vector<unsigned short> ret = {m_registers[Reg::PLC_ARS1], m_registers[Reg::PLC_ARS2], m_registers[Reg::PLC_ARS3], m_registers[Reg::PLC_ARS4]};
	return ret;
}

void TreadmillDriver::checkZeroPoint() {
	LD("TRACE angle: " + std::to_string(getAngle()) + "    speed: " + std::to_string(getActualSpeed()));
	if (isReady() && !isSettingPoint() && getAngle() < 100 && getActualSpeed() < 4) {
		m_zero_point = true;
		if (m_sas_accel_restore) {
			ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_accel) * 10);
			ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(ctx.m_decel) * 10);
			m_sas_accel_restore = false;
		}
	} else {
		m_zero_point = false;
	}
}

bool TreadmillDriver::isSasExists() {
	return m_sas_exists;
}

void TreadmillDriver::setSasMode(unsigned short value) {
	m_queue.putEvent(Command{WRITE, Reg::PLC_SAS_MODE, {value}});
	m_sas_mode = value;
}

int TreadmillDriver::getSasMode() const {
	return m_sas_mode;
}

int TreadmillDriver::getSasSymmetry() const {
	return m_sas_symmetry;
}

int TreadmillDriver::getSasStepLength() const {
	return m_sas_step_length;
}

int TreadmillDriver::getTreadmillType() const {
	return m_treadmill_type > 0 ? m_treadmill_type : 100;
}

std::string TreadmillDriver::getRunStateName(RunState state) {
	switch (state) {
	case RunState::Pause:
		return "Pause";
	case RunState::Run:
		return "Run";
	case RunState::Stop:
		return "Stop";
	}
	return "Unknown";
}

bool TreadmillDriver::isSasTestNeeded() const {
	return !(m_registers.at(Reg::PLC_SAS_STATUS) & 0xE000);
}

bool TreadmillDriver::isSasTestInitiated() const {
	return m_registers.at(Reg::PLC_SAS_MODE) & 0x8000;
}


bool TreadmillDriver::isSasTestCompletedWithLimitation() const {
	return m_registers.at(Reg::PLC_SAS_STATUS) & 0x4000;
}

bool TreadmillDriver::isSasTestCompletedSuccessfull() const {
	return m_registers.at(Reg::PLC_SAS_STATUS) & 0x2000;
}

bool TreadmillDriver::isSasTestCompletedWithError() const {
	return m_registers.at(Reg::PLC_SAS_STATUS) & 0x8000;
}

void TreadmillDriver::stopSasTest() {
	m_queue.putEvent(Command{READ, Reg::PLC_SAS_MODE, {}, [this](std::vector<unsigned short> v) {
			m_queue.putAsNext(Command{WRITE, Reg::PLC_SAS_MODE, {static_cast<unsigned short>(v.at(0) & ~0x8000)}});
			m_registers[Reg::PLC_SAS_MODE] = v.at(0) & ~0x8000;
		}
	});
}
