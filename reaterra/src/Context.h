#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "BluetoothInterface.h"
#include "DbConnector.h"
#include "DeviceMonitor.h"
//#include "DisplayLightnessPwm.h"
#include "Event.h"
#include "GraphicEngine.h"
#include "HrMonitor.h"
#include "Logger.h"
#include "PersistedParameter.h"
#include "PictureManager.h"
#include "RunSession.h"
#include "SoundInterface.h"
#include "SyncQueue.h"
#include "TouchScreenDriver.h"
#include "TitabitInterface.h"
#include "TreadmillDriver.h"
#include "WiFiRemote.h"

/// Контекст среды исполнения программы.
class Context {

public:

	static const char PARAMS_SEPARATOR = '^';
	PersistedParameter m_settings; 		///< Управление настройками.
	DbConnector m_db;					///< Доступ к БД.
//	DisplayLightnessPwm m_dlp;			///< ШИМ для настройки яркости дисплея.
	SyncQueue<std::unique_ptr<Event>> m_queue {"Main Loop"};	///< Очередь сообщений.
	SoundInterface m_sound;				///< Звуковой интерфейс.
	DeviceMonitor m_dev_mon;			///< Управление устройствами.
	TreadmillDriver m_treadmill;		///< Интерфейс с дорожкой.
	GraphicEngine m_graphic;			///< Графическая система.
	PictureManager m_pic_mgr;			///< Управление картинками.
	TouchScreenDriver m_touch;			///< Сенсорный ввод.
	HrMonitor m_hr_monitor;				///< Датчик пульса.
	RunSession m_session;				///< Беговая сессия.
	WiFiRemote m_remote;				///< Интерфейс с планшетом.
	TitabitInterface m_titabit;			///< Интерфейс с визуализатором.

	std::string m_patient;				///< Текущий пациент.
	int m_patient_id;					///< Идентификатор текущего пациента.
	int m_profile_id;					///< Идентификатор профиля пациента.
	int m_accel;						///< Уровень ускорения по умолчанию.
	int m_decel;						///< Уровень торможения по умолчанию.
	std::string m_admin;				///< Текущий пароль.
	float m_patient_weight;				///< Вес пациента.
	bool m_notuse_doc_password;			///< Флаг отмены пароля врача.
	bool m_first_run;					///< Флаг первого запуска системы.
	bool m_run_sas_test = false;		///< Флаг запуска дорожки во время САС теста.
	std::vector<std::string> m_menu_history;
	std::map<std::string, std::vector<std::string>> m_prev_menu; 	///< Контекст предыдущего меню
	static std::string main_board;

	Context();				///< Конструктор
	virtual ~Context();		///< Деструктор
};

extern Context ctx;

#endif /* CONTEXT_H_ */
