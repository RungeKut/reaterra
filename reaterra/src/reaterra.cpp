//#include <exception>
#include <fcntl.h>
#include <signal.h>

#include "Context.h"
#include "Locale.h"
#include "MainLoop.h"
#include "Styles.h"

Logger& logger_ = Logger::instance("ReaTerra", Logger::LogLevel::Debug, false);
Context ctx;
Locale& locale = Locale::instance();

/*!
 * @brief Обработчик системных прерываний.
 *
 * @param sig полученное прерывание.
 */
void  INThandler(int sig) {
	signal(sig, SIG_IGN);
	ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Quit));
}


/*!
 * @brief Главная функция приложения.
 *
 * @return код возврата.
 */
int main() {
	int ret = 0;
	signal(SIGINT, INThandler);
	locale.init(ctx.m_settings.getParamString("locale"));

	//TODO: Убрать инициализацию кнопки НАЗАД из главной функции.
	back_style.text = M("НАЗАД");
	MainLoop m_loop;
	try {
		ret = m_loop.loop();
	} catch (std::exception &e) {
		Logger::instance().log(std::string("Exception happens: ") + e.what());
		Logger::instance().flush();
	} catch (...) {
		Logger::instance().log("Exception happens: unknown");
		Logger::instance().flush();
		throw;
	}

	return ret;
}
