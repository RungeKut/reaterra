#include "TestMenu.h"

#include "Context.h"

TestMenu::TestMenu()
:	m_timer {[this](int id){ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Timer, "TST"));}, 0},
	m_cnt {0}
{
	LD("Ctor");
	m_background_id = ctx.m_pic_mgr.loadPicture("test_red.png");
	WTFStyle style {default_style};
	m_message = addWidget(std::make_shared<WidgetTextField>(10, 10, 1200, 600, style, M("КРАСНЫЙ")));
	m_color = M("КРАСНЫЙ");

	auto drives = ctx.m_dev_mon.getUsbDrives();
	if (drives.size() && ctx.m_dev_mon.getUsbDrives().back().isValid())
	{
		m_flash = M("FLASH-накопитель подключен: ") + std::to_string(ctx.m_dev_mon.getUsbDrives().back().getFreeSpace() / 1024) + " " + M("Кбайт");
	} else {
		m_flash = M("FLASH-накопитель отключен");
	}

	ctx.m_dev_mon.setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)> ([this](auto s, auto u){
		if (s == "add") {
			m_flash = M("FLASH-накопитель подключен: ") + std::to_string(u.getFreeSpace() / 1024) + " " + M("Кбайт");
		} else if (s == "remove") {
			m_flash = M("FLASH-накопитель отключен");
		}
	}));

	m_timer.startTimerMillis(1000);
}

TestMenu::~TestMenu() {
	m_timer.stopTimer();
	ctx.m_dev_mon.setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)>());
	LD("Dtor");
}

Menu* TestMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;
	int touch_x {-1}, touch_y {-1};
	std::string sound;
	if (event->getType() == Event::EventType::Timer && event->getExtra() == "TST") {
		if (m_cnt == 0) {
			m_color = M("КРАСНЫЙ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_red.png");
		} else if (m_cnt == 5) {
			m_color = M("ЗЕЛЕНЫЙ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_green.png");
			sound = M("ЗВУК");
			ctx.m_sound.play(SoundInterface::Sound::Info);
		} else if (m_cnt == 10) {
			m_color = M("СИНИЙ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_blue.png");
		} else if (m_cnt == 15) {
			m_color = M("БЕЛЫЙ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_white.png");
			sound = M("ЗВУК");
			ctx.m_sound.play(SoundInterface::Sound::Info);
		} else if (m_cnt == 20) {
			m_color = M("ЧЕРНЫЙ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_black.png");
		} else if (m_cnt == 25) {
			m_color = M("ГРАДИЕНТ");
			m_background_id = ctx.m_pic_mgr.loadPicture("test_gradient.png");
		} else if (m_cnt == 35){
			m_cnt = -1;
		}

		m_cnt++;
	}

	if (event->getType() == Event::EventType::Touch){
		ctx.m_sound.play(SoundInterface::Sound::Click);
		auto tev =  dynamic_cast<TouchEvent*>(event.get());
		touch_x = tev->getX();
		touch_y = tev->getY();
		m_touch = "X:" + std::to_string(touch_x) + "   Y:" + std::to_string(touch_y);
	} else {
		m_touch = "";
	}

	m_message->setText(sound + "\\" + m_color + "\\" + m_flash + "\\" + m_touch);
	show(false);

	if (touch_x >= 0){
		ctx.m_graphic.drawRectangle(touch_x, touch_y, 20, 20);
		touch_x = touch_y = -1;
	}

//	LD("tick: " + std::to_string(m_cnt));
	ctx.m_graphic.show();
	return ret;
}
