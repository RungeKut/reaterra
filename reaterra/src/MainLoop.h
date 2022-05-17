#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <atomic>

#include "Context.h"
#include "Menu.h"
#include "SyncQueue.h"
#include "TouchEvent.h"

/// Главный цикл приложения.
class MainLoop {

public:

	/// Конструктор.
	MainLoop();

	/// Деструктор.
	virtual ~MainLoop();

	/*!
	 * @brief Функция цикла.
	 *
	 * @return код возврата.
	 */
	int loop();


private:
	std::atomic_bool m_running;
	std::unique_ptr<Menu> m_current_menu;
	void changeHistory(Menu &menu);

};

#endif /* MAINLOOP_H_ */
