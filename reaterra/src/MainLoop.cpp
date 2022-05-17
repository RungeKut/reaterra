#include "MainLoop.h"

#include <memory>

#include "Event.h"
#include "MainMenu.h"
#include "TestMenu.h"

MainLoop::MainLoop()
:	m_running {true}
{
	LD("Ctor");

	for (int i = 0; i < 30 && !ctx.m_dev_mon.isDriveChecked(); ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	auto drives = ctx.m_dev_mon.getUsbDrives();
	if (ctx.m_settings.getParamInt("test_mode") != 0
			|| (drives.size()
					&& ctx.m_dev_mon.getUsbDrives().back().isValid()
					&& ctx.m_dev_mon.getUsbDrives().back().isFileExisted("test.txt")))
	{
		m_current_menu = std::make_unique<TestMenu>();
	} else {
		m_current_menu = std::make_unique<MainMenu>();
	}
	changeHistory(*m_current_menu);

}

MainLoop::~MainLoop() {
	LD("Dtor");
}

int MainLoop::loop() {
	int ret = 0;
	std::unique_ptr<Menu> menu;
	ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	while (m_running) {
		auto event = ctx.m_queue.getEvent();
//		LD("Got event: " + event->getName());

		if ((*event).getType() == Event::EventType::Quit) {
			m_running = false;
			LD("Quit event received");
			break;
		} else if ((*event).getType() == Event::EventType::Shutdown) {
			m_running = false;
			LD("Shutdown event received");
			ret = 19;
			break;
		} else if ((*event).getType() == Event::EventType::Restart) {
			m_running = false;
			LD("Restart event received");
			ret = 20;
			break;
		}

		menu.reset((*m_current_menu).processEvent(std::move(event)));
		if (menu.get()) {
			m_current_menu.reset(menu.release());
			changeHistory(*m_current_menu);
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	}

	return ret;
}

void MainLoop::changeHistory(Menu& menu) {
	std::string buf;
	for (auto const s: ctx.m_menu_history) {
		buf += s + "  ";
	}
	LD("Hist_queue before: " + buf);

	std::string menu_type {Utils::typeName(typeid(menu))};

	unsigned i = 0;
	for (; i < ctx.m_menu_history.size(); ++i) {
		if (ctx.m_menu_history.at(i) == menu_type) {
			break;
		}
	}

	if (i < ctx.m_menu_history.size()) {
		if (i != ctx.m_menu_history.size() - 1) {
			ctx.m_menu_history.erase(ctx.m_menu_history.begin() + i + 1, ctx.m_menu_history.end());
		}
	} else {
		ctx.m_menu_history.push_back(menu_type);
	}

	buf = "";
	for (auto const s: ctx.m_menu_history) {
		buf += s + "  ";
	}
	LD("Hist_queue after: " + buf);
}
