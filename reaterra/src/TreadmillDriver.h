#ifndef TREADMILLDRIVER_H_
#define TREADMILLDRIVER_H_

#include <map>

#include "modbus.h"

#include "SyncQueue.h"
#include "ThreadClass.h"
#include "Timer.h"

/// Драйвер дорожки.
class TreadmillDriver : public ThreadClass {

public:

	/// Состояние дорожки.
	enum class RunState {
		Stop,	///< Остановлена.
		Run,	///< Бег.
		Pause	///< Пауза.
	};

	/// Направление движения.
	enum class Direction {
		Forward,	///< Вперед.
		Backward	///< Назад.
	};

	///< Управление подъемником.
	enum class Lift {
		Stop,	///< Остановлен.
		Up,		///< Подъем.
		Down	///< Опускание.
	};

	/// Управление внешней стресс-системой.
	enum class ExtMode {
		On = 1,		///< Включена.
		Off = 2		///< Отключена.
	};

	/// Регистры.
	enum class Reg {
		PLC_ID = 0, 	///< Идентификатор.
		PLC_STAT = 1,	///< Статус системы.
		PLC_CS = 2,		///< Регистр управления системой.
		PLC_CD = 3,		///< Регистр управления приводом.
		PLC_CTA = 4,	///< Регистр задания времени ускорения.
		PLC_CTD = 5,	///< Регистр задания времени торможения.
		PLC_SD = 6,		///< Регистр состояния привода.
		PLC_ED = 7,		///< Регистр ошибки привода.
		PLC_CL = 8,		///< Регистр управления подъемником.
		PLC_AX = 9,		///< Регистр данных угла.
		PLC_ACX = 10,	///< Регистр калибровочных данных угла Х.
		PLC_CAL = 11,	///< Регистр калибровки скорости.
		PLC_ACY_V0 = 11,	///< Регистр калибровочных данных угла Y.
		PLC_TYPE = 12,		///< Регистр определения типа/скорости дорожки.
		PLC_CAL_V0 = 12,	///< Регистр калибровки скорости.
		PLC_SAS_STEP = 13,	///< Регистр длины шага САС.
		PLC_SAS_SYM_LEFT = 14,	///< Регистр симметрии САС для левой ноги.
		PLC_SAS_MODE = 16,	///< Регистр выбора режима САС.
		PLC_SAS_STATUS = 17,  ///< Регистр состояния САС.
		PLC_EX = 18,		///< Регистр управления внешней стресс системой.
		PLC_UNL_BATT = 19,	///< Регистр уровня заряда батареи разгрузки.
		PLC_CONF = 21,		///< Регистр конфигурации дорожки.
		PLC_VER = 22,		///< Регистр версии прошивки контроллера дорожки.
		PLC_ARS1 = 24, 		///< Регистр состояния датчиков САС
		PLC_ARS2 = 25, 		///< Регистр состояния датчиков САС
		PLC_ARS3 = 26, 		///< Регистр состояния датчиков САС
		PLC_ARS4 = 27, 		///< Регистр состояния датчиков САС
		PLC_EX_V0 = 78,		///< Регистр управления внешней стресс системой (для версии контроллера V0).
	};

	/*!
	 * @brief Получение строки названия состояния дорожки.
	 *
	 * @param state состояние дорожки.
	 * @return строка названия.
	 */
	static std::string getRunStateName(const RunState state);

	/*!
	 * @brief Получение значения ускорения по уровню.
	 *
	 * @param id уровень ускорения (1-7).
	 * @return значение ускорения.
	 */
	int getAccelValue(const int id);

	/*!
	 * @brief Получение времени разгона/торможения по уровню.
	 *
	 * @param id уровень.
	 * @return время разгона/торможения в секундах.
	 */
	static int getAccelTimeValue(const int id);

	/// Конструктор.
	TreadmillDriver();

	/// Деструктор.
	virtual ~TreadmillDriver();

	/*!
	 * Функция записи в лог файл для библиотеки Modbus.
	 *
	 * @param level уровень сообщения.
	 * @param msg текст сообщения.
	 */
	void log(char level, std::string msg);

	/*!
	 * @brief Получение текущего ускорения.
	 *
	 * @return ускорение.
	 */
	int getAcceleration() const;

	/*!
	 * @brief Установка ускорения.
	 *
	 * @param acceleration ускорение.
	 */
	void setAcceleration(int acceleration);

	/*!
	 * @brief Получение угла наклона.
	 *
	 * @return угол наклона.
	 */
	int getAngle() const;

	/*!
	 * @brief Установка угла наклона.
	 *
	 * @param angle угол наклона.
	 */
	void setAngle(int angle, bool not_set = false);

	/*!
	 * @brief Получение текущего значения торможения.
	 *
	 * @return значение торможения.
	 */
	int getDeceleration() const;

	/*!
	 * @brief Установка текущего значения торможения.
	 *
	 * @param deceleration значение торможения.
	 */
	void setDeceleration(int deceleration);

	/*!
	 * @brief Получение текущего направления движения.
	 *
	 * @return текущее направление движения.
	 */
	Direction getDirection() const;

	/*!
	 * @brief Установка текущего направления движения.
	 *
	 * @param direction направление движения
	 */
	void setDirection(Direction direction);

	/*!
	 * @brief Получение текущей скорости движения.
	 *
	 * @return текущая скорость движения.
	 */
	int getSpeed() const;

	/*!
	 * @brief Установка текущей скорости движения.
	 *
	 * @param speed скорость движения.
	 */
	void setSpeed(int speed);

	/*!
	 * @brief Получение состояния дорожки.
	 *
	 * @return состояние дорожки.
	 */
	RunState getState() const;

	/*!
	 * @brief Установка состояния дорожки.
	 *
	 * @param state состояние дорожки.
	 */
	void setState(RunState state, bool set_cl = true);

	/// Переключение в режим калибровки и обратно.
	void toggleMode();

	/*!
	 * @brief Определение готовности дорожки.
	 *
	 * @return true: дорожка готова.
	 */
	bool isReady() const;

	/*!
	 * @brief Установка подъемника в режиме пульта.
	 *
	 * @param lift режим подъемника.
	 */
	void setLift(Lift lift);

	/*!
	 * @brief Запись в регистр дорожки.
	 *
	 * @param reg номер регистра.
	 * @param value значение.
	 */
	void setRegister(Reg reg, int value);

	/*!
	 * @brief Чтение регистра дорожки.
	 *
	 * @param reg номер регистра.
	 * @return значение.
	 */
	int getRegister(Reg reg);

	/*!
	 * @brief Включение/выключение режима пульта.
	 *
	 * @param enable true: включить, false: выключить.
	 */
	void setRemoteControlLifting(bool enable);

	/*!
	 * @brief Проверка наличия ошибки заступа.
	 *
	 * @return true: есть ошибка, false: ошибки нет.
	 */
	bool isCrossError() const;

	/*!
	 * @brief Проверка состояния аварийной кнопки.
	 *
	 * @return true: кнопка нажата, false: кнопка не нажата.
	 */
	bool isEmegencyStop() const;

	/*!
	 * @brief Проверка наличия связи с дорожкой.
	 *
	 * @return true: связь есть, false: связи нет.
	 */
	bool isPlcConnected();

	/*!
	 * @brief Проверка работы преобразователя частоты.
	 *
	 * @return true: преобразователь работает, false: преобразователь не работает
	 */
	bool isPFreqAlarm();

	/*!
	 * @brief Проверка наличия связи с подъемником.
	 *
	 * @return @return true: связь есть, false: связи нет.
	 */
	bool isFallenOnTread();

	/*!
	 * @brief Проверка ошибки частотника.
	 *
	 * @return @return true: ошибка есть, false: ошибки нет.
	 */
	bool isPFreqError();

	/*!
	 * @brief Проверка перегрузки актуатора.
	 *
	 * @return @return true: перегрузка есть, false: перегрузки нет.
	 */
	bool isActuatorOverloaded();

	/*!
	 * @brief Проверка состояния калибровки.
	 *
	 * @return true: режим калибровки включен, false: калибровка выключена.
	 */
	bool isCalibrating();

	/// Сброс ошибок.
	void resetError();

	/// Сброс ошибки частотника.
	void resetPfError();

	/// Сброс ошибки актуатора.
	void resetActuatotError();

	/// Сброс ошибки падения.
	void resetFallenOnTreadError();

	/*!
	 * @brief Получение максимальной скорости дорожки вперед.
	 *
	 * @return максимальная скорость дорожки вперед.
	 */
	int getMaxForwardSpeed();

	/*!
	 * @brief Получение максимальной скорости дорожки назад.
	 *
	 * @return максимальная скорость дорожки назад.
	 */
	int getMaxBackwardSpeed();

	/*!
	 * @brief Получение минимальной скорости дорожки.
	 *
	 * @return минимальная скорость дорожки.
	 */
	int getMinSpeed();

	/*!
	 * @brief Получение максимального угла подъема.
	 *
	 * @return максимальный угол подъема.
	 */
	int getMaxAngle();

	/*!
	 * @brief Получение шага изменения скорости.
	 *
	 * @return шаг изменения скорости.
	 */
	int getSpeedStep();

	/*!
	 * @brief Получение угла калибровки по оси X.
	 *
	 * @return угол калибровки.
	 */
	int getCalibrationAngleX();

//	/*!
//	 * @brief Получение угла калибровки по оси Y.
//	 *
//	 * @return угол калибровки.
//	 */
//	int getCalibrationAngleY();

	/*!
	 * @brief Установка режима работы с внешней стресс-системой.
	 *
	 * @param mode режима работы с внешней стресс-системой.
	 */
	void setExternal(ExtMode mode);

	/*!
	 * @brief Получение значения регистра.
	 *
	 * @param name идентификатор регистра.
	 * @return значение регистра.
	 */
	uint16_t getRegisterState(Reg name);

	/*!
	 * @brief Получение реальной скорости дорожки.
	 *
	 * @return реальная скорость дорожки.
	 */
	int getActualSpeed() const;

	/*!
	 * @brief Проверка, что дорожка находится в исходном состоянии.
	 *
	 * @return true: исходное состояние, false: не в исходном состоянии.
	 */
	bool isZeroPoint() const;

	/// Перевод дорожки в исходное состояние.
	void doZeroPoint();

	/*!
	 * @brief Управление частотой калибровки скорости.
	 *
	 * @param value true: включить частоту калибровки, false: отключить частоту калибровки.
	 */
	void setDNF(bool value);

	/*!
	 * @brief Установка калибровочного коэффициента.
	 *
	 * @param value калибровочный коэффициент.
	 */
	void setSpeedCalib(unsigned short value);

	/// Сброс калибровки подъемника.
	void actuatorCalibReset();

	/// Установка калибровки подъемника.
	void actuatorCalibSet();

	/*!
	 * @brief Проверка готовности подъемника.
	 *
	 * @return true: подъемник готов, false: подъемник в процессе работы.
	 */
	bool isSettingPoint();

	/*!
	 * @brief Проверка наличия разгрузки.
	 *
	 * @return true: разгрузка присутствует, false: разгрузка отсутствует.
	 */
	bool isUnloadingExists();

	/*!
	 * @brief Проверка наличия САС.
	 *
	 * @return true: система САС присутствует, false: САС отсутствует.
	 */
	bool isSasExists();

	/*!
	 * @brief Установка режима работы САС.
	 *
	 * @param value режим работы САС.
	 */
	void setSasMode(unsigned short value);

	/*!
	 * @brief Получение режима работы САС.
	 *
	 * @return режим работы САС.
	 */
	int getSasMode() const;

	/*!
	 * @brief Получение симметричности САС.
	 *
	 * @return данные симметричности САС.
	 */
	int getSasSymmetry() const;

	/*!
	 * @brief Получение длины шага САС.
	 *
	 * @return данные длины шага САС.
	 */
	int getSasStepLength() const;

	/*!
	 * @brief Получение типа дорожки - по сути максимальную скорость в км/ч.
	 *
	 * @return тип дорожки.
	 */
	int getTreadmillType() const;

	/*!
	 * @brief Проверка ошибки перегрева двигателя.
	 *
	 * @return true: ошибка есть, false: ошибки нет.
	 */
	bool isDriveOverheatError() const;

	/*!
	 * @brief Признак того, что произошло падение.
	 *
	 * @return true: было падение, false: падения не было.
	 */
	bool isFallHappen() const;

	/// Сброс ошибки падения.
	void resetFallenError();

	/*!
	 * @brief Проверка наличия оптической помехи.
	 *
	 * @return true: помеха есть, false: помехи нет.
	 */
	bool isOpticError() const;

	/// Сброс ошибки оптической помехи.
	void resetOpticError();

	/*!
	 * @brief Проверка ошибки связи с САС.
	 *
	 * @return true: ошибка есть, false: ошибки нет.
	 */
	bool isSasConnectionError() const;

	/*!
	 * @brief Проверка установки ног в начальную позицию при САС ходьбе.
	 *
	 * @return true: готовность есть, false: готовности нет.
	 */
	bool isFeetReady() const;

	/*!
	 * @brief Проверка что человек находится на полотне при САС ходьбе.
	 *
	 * @return true: на полотне, false: не на полотне.
	 */
	bool isSasManOnTread() const;

	/// Остановка обмена с контроллером дорожки.
	void stopInteraction();

	/// Возобновление обмена с контроллером дорожки.
	void startInteraction();

	/*!
	 * @brief Получение версии прошивки контроллера.
	 *
	 * @return версия прошивки.
	 */
	int getControllerVersion();

	/*!
	 * @brief Получение уровня заряда батареи разгрузки в процентах.
	 *
	 * @return уровень заряда.
	 */
	int getUnlBattLevel() const;

	/*!
	 * @brief Проверка ошибки зарядки батареи разгрузки.
	 *
	 * @return true: ошибка есть, false: ошибки нет.
	 */
	bool isUnlBattWarn() const;

	/*!
	 * @brief Проверка завершения инициализации дорожки.
	 *
	 * @return true: инициализация завершена.
	 */
	bool isInitCompleted() const;

	/*!
	 * @brief Проверка необходимости самотестирования САС.
	 *
	 * @return true: самотестирование необходимо.
	 */
	bool isSasTestNeeded() const;

	/*!
	 * @brief Проверка того,что самотестирование САС инициировано.
	 *
	 * @return true: самотестирование инициировано.
	 */
	bool isSasTestInitiated() const;

	/*!
	 * @brief Проверка того, что самотестирование САС завершено с ограничением режимов.
	 *
	 * @return true: самотестирование завершено с ограничением режимов.
	 */
	bool isSasTestCompletedWithLimitation() const;

	/*!
	 * @brief Проверка того, что самотестирование САС завершено успешно.
	 *
	 * @return true: самотестирование завершено успешно
	 */
	bool isSasTestCompletedSuccessfull() const;

	/*!
	 * @brief Проверка того, что самотестирование САС завершено с ошибкой.
	 *
	 * @return true: самотестирование завершено с ошибкой.
	 */
	bool isSasTestCompletedWithError() const;

	/*!
	 * @brief Получение состояния сенсоров САС.
	 *
	 * @return int[4]: состояние сенсоров САС.
	 */
	std::vector<unsigned short> getSasSensorsStatus();

	/*!
	 * @brief Запуск режима самотестирования САС.
	 *
	 * @param stat_test true - статический тест, иначе - динамический.
	 */
	void startSasTest(const bool stat_test = false);

	/*!
	 * @brief Перезапуск режима самотестирования САС.
	 */
	void restartSasTest();

	/// Остановка режима самотестирования САС.
	void stopSasTest();


	bool m_read_sas_service_regs = false;

private:

	struct Command {
		bool read;
		Reg regnum;
		std::vector<unsigned short> data;
		std::function<void(std::vector<unsigned short>)> on_read;
		unsigned qnt = 1;

	};

	const static bool READ = true;
	const static bool WRITE = false;
	const static int MODBUS_FAIL = -1;
	const static int HEART_BEAT_VALUE = 150;
	const static int HEART_BEAT_TIMEOUT = 5000;

	modbus_t *m_modbus;
	Timer m_timer;
	SyncQueue<Command> m_queue;
	int m_poll_cnt_max;
	int m_poll_cnt;
	int m_modbus_attempt_max;
	bool m_connected;
	int m_queue_limit;
	Direction m_direction;
	int m_speed;
	int m_angle;
	int m_acceleration;
	int m_deceleration;
	bool m_ready;
	RunState m_state;
	bool m_cmd_in_progress;
	bool m_tele;
	bool m_emegency_stop;
	bool m_cross_error;
	bool m_plc_connected;
	bool m_pfreq_connected;
	bool m_fall_on_tread;
	bool m_calibration;
	int m_calib_angle_x;
	int m_calib_angle_y;
	int m_actual_speed;
	bool m_zero_point;
	bool m_state_update;
	bool m_set_point;
	bool m_remote;
	int m_paused_angle;
	int m_paused_speed;
	bool m_paused;
	bool m_actuator_overload;
	bool m_pfreq_error;
	bool m_drive_overheat_error;
	bool m_fall_happen;
	bool m_optic_error;
	bool m_sas_conn_error;
	bool m_is_feet_ready;
	bool m_is_man_on_tread;
	std::map<Reg, uint16_t> m_registers;
	bool m_sas_exists;
	int m_sas_mode;
	int m_treadmill_type;
	bool m_first_run;
	bool m_interatrion;
	int m_sas_symmetry;
	int m_sas_step_length;
	int m_controller_version;
	int m_controller_config;
	bool m_init_waiting;
	int m_unl_batt_level;
	bool m_unl_batt_warn;
	bool m_unl_exists;
	bool m_init_completed;
	bool m_sas_test_complete = false;
	bool m_sas_accel_restore = false;
	unsigned short m_sas_test_result = 0;

	bool process() override;
	bool init() override;
	bool isConnected() const;
	std::string getRegisterName(const Reg regnum) const;
	void initTreadmill();
	void checkZeroPoint();
};

#endif /* TREADMILLDRIVER_H_ */
