#ifndef WIDGETIMAGEBUTTON_H_
#define WIDGETIMAGEBUTTON_H_

#include "Widget.h"

#include <functional>
#include <string>

#include "SoundInterface.h"
#include "Timer.h"
#include "TouchEvent.h"
#include "WidgetTextField.h"


/// Виджет кнопки с картинкой.
class WidgetImageButton: public Widget {

public:

	/// Событие.
	enum class Event {
		Click,		///< Щелчок.
		Press,		///< Нажатие.
		Release		///< Отпускание.
	};

	/// Тип.
	enum class Type {
		Clicker,		///< Кнопка-щелчок.
		PressRelease,	///< Нажатие-отпускание.
		Generator,		///< Кнопка с повтором.
		Trigger		///< Кнопка-триггер.
	};


	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param active_file картинка активного состояния.
	 * @param disabled_file картинка заблокированного состояния.
	 * @param passive_file картинка пассивного состояния.
	 * @param on_event функция обработки событий.
	 * @param type тип кнопки.
	 * @param text текстовый виджет, связанный с кнопкой.
	 */
	WidgetImageButton(int x, int y, std::string active_file, std::string disabled_file = "", std::string passive_file = "",
			std::function<std::unique_ptr<::Event> (Event)> on_event = {}, Type type = Type::Clicker,
			WidgetTextFieldSP text = WidgetTextFieldSP());

	/// Деструктор.
	virtual ~WidgetImageButton();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(::Event *event) override;

	/// Отрисовка изображения.
	void show() override;

	/*!
	 * Установка картинки пассивного состояния.
	 *
	 * @param file картинка пассивного состояния.
	 */
	void setPassive(std::string file);

	/*!
	 * Установка картинки активного состояния.
	 *
	 * @param file картинка активного состояния.
	 */
	void setActive(std::string file);

	/*!
	 * Установка картинки заблокированного состояния.
	 *
	 * @param file картинка заблокированного состояния.
	 */
	void setDisabled(std::string file);

	/// Очистка кнопки-триггера.
	void clearTrigger();

	/// Установка кнопки-триггера.
	void setTrigger();

	/*!
	 * Установка состояния.
	 *
	 * @param state состояние.
	 * @return ссылка на виджет.
	 */
	WidgetImageButton *setState(State state) override;

	/*!
	 * Проверка состояния кнопки-триггера.
	 *
	 * @return true: установлена, false: сброшена.
	 */
	bool isSet();

	/*!
	 * Установка параметров кнопки с повтором.
	 *
	 * @param delay задержка перед повтором.
	 * @param step период повтора.
	 */
	void setRepeat(int delay, int step);

	/*!
	 * Установка звука на кнопку.
	 *
	 * @param sound идентификатор звука.
	 */
	void setSound(SoundInterface::Sound sound);

	WidgetImageButton(const WidgetImageButton&) = delete;

private:

	enum class Repeat {
		Stop,
		Delay,
		Run
	};

	int m_active_id;
	int m_passive_id;
	int m_disabled_id;
	std::function<std::unique_ptr<::Event> (Event)> m_on_event;
	Type m_type;
	bool m_trigger;
	int m_current_image;
	Timer m_repeat_timer;
	int m_repeat_delay;
	int m_repeat_step;
	Repeat m_repeat_state;
	SoundInterface::Sound m_sound;
	WidgetTextFieldSP m_text;

	void onRepeatTimer();


};

typedef std::shared_ptr<WidgetImageButton> WidgetImageButtonSP;
typedef std::function<std::unique_ptr<::Event> (WidgetImageButton::Event)> WIBFunc;


#endif /* WIDGETIMAGEBUTTON_H_ */
