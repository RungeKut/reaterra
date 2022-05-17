#ifndef EVENT_H_
#define EVENT_H_

#include <memory>
#include <string>

/// События.
class Event {

public:

	/// Типы событий.
	enum class EventType {
		Quit, 					///< Выход.
		Touch,					///< Касание сенсора.
		Timer,					///< Событие таймера.
		TreadmillConnected,		///< Дорожка подключена.
		TreadmillReady,			///< Дорожка готова.
		TreadmillDisconnected,	///< Дорожка отключена.
		Refresh,				///< Обновление.
		Menu,					///< Отобразить меню.
		ModalYes,				///< Нажата кнопка ДА.
		ModalNo,				///< Нажата кнопка НЕТ.
		ModalCancel,			///< Нажата кнопка ОТМЕНА.
		Zastup,					///< Произошел заступ.
		EmergencyButton,		///< Нажата аварийная кнопка.
		KbdEvent,				///< Событие клавиатуры.
		HrMonitorEvent,			///< Событие монитора пульса.
		Shutdown,				///< Выключение.
		Restart,				///< Перезагрузка.
		IconClick				///< Щелчок по иконке в заголовке диалога.
	};

	/*!
	 * @brief Конструктор.
	 *
	 * @param type тип события.
	 * @param extra дополнительные данные.
	 */
	Event(EventType type, std::string extra = {});

	/// Деструктор.
	virtual ~Event();

	/*!
	 * @brief Получение типа события.
	 *
	 * @return тип события.
	 */
	EventType getType() const;

	/*!
	 * @brief Получение название события.
	 *
	 * @return название события.
	 */
	std::string getName();

	/*!
	 * @brief Получение дополнительных данных.
	 *
	 * @return дополнительные данные.
	 */
	std::string getExtra();

private:
	EventType m_type;
	std::string m_extra;
};

typedef std::unique_ptr<Event> EventUP;

#endif /* EVENT_H_ */
