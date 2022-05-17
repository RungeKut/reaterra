#include "SettingsExportMenu.h"

#include "Context.h"
#include "MainMenu.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "SettingsMenu.h"
#include "WidgetTextField.h"
#include "WidgetBreadCrumbs.h"

SettingsExportMenu::SettingsExportMenu()
:	Menu {0, 0, "settings_export_menu.png", {}}
{
	LD("Ctor");

	addBreadCrumps(std::vector<WBCElement> {
		{M("НАЧАЛО"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<MainMenu>());}, bc_link_style},
		{M("НАСТРОЙКИ"), [this](WidgetImageButton::Event e){return std::make_unique<MenuEvent>(std::make_unique<SettingsMenu>(ctx.m_admin));}, bc_link_style},
		{M("импорт и экспорт данных"), WIBFunc(), bc_last_style}
	});

	m_patients_btn->setState(Widget::State::Disabled);
	m_hr_btn->setState(Widget::State::Disabled);

	WTFStyle style {default_style};
	style.kbd_type = KbdType::None;
	style.font_style = FontStyle::Normal;
	style.text_color_passive = to_base(Colors::White);
	style.text_color_active = to_base(Colors::BlueOnYellow);
	style.text_color_disabled = to_base(Colors::White);
	style.halign = Align::Center;
	style.lines = 6;
	style.hpad = 0;
	style.font_size = 26;
	style.rect_color_active = to_base(Colors::None);;


	m_export = addWidget(std::make_shared<WidgetImageButton>(146, 496, "settings_export_active.png", "settings_export_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				bool files_exist = false;
				if (ctx.m_dev_mon.getUsbDrives().back().isValid()) {
					if(ctx.m_dev_mon.getUsbDrives().back().isFileExisted("reaterra_db_backup.sql.gz")
							|| ctx.m_dev_mon.getUsbDrives().back().isFileExisted("out.emi")
							|| ctx.m_dev_mon.getUsbDrives().back().isFileExisted("history.db")
							) {
						files_exist = true;
					}
				}
				if(files_exist) {
					LD("Files exist modal");
					using Flags = ModalCommonDialog::Flags;
					m_modal = std::make_unique<ModalCommonDialog>(M("Экспорт данных"),
							M("Файлы экспорта уже есть\\на носителе. Перезаписать?"), "EDEX", Flags::GreenTitle | Flags::YesCancel,
							"icon_save_green.png");
				} else {
					doExport();
				}

				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(416, 496, 213, 184, style, M("ЭКСПОРТ"))));

	m_import = addWidget(std::make_shared<WidgetImageButton>(650, 496, "settings_import_active.png", "settings_import_disabled.png", "",
			[this](WidgetImageButton::Event e) {
				if (ctx.m_dev_mon.getUsbDrives().back().isValid()) {
					using Flags = ModalCommonDialog::Flags;
					if (ctx.m_dev_mon.getUsbDrives().back().isFileExisted("reaterra_db_backup.sql.gz")) {
						ctx.m_dev_mon.getUsbDrives().back().driveMount();
						ctx.m_dev_mon.getUsbDrives().back().fileCopy("/media/usb/reaterra_db_backup.sql.gz", "reaterra_db_backup.sql.gz");
						ctx.m_db.restoreDB("reaterra_db_backup.sql.gz");
						ctx.m_dev_mon.getUsbDrives().back().driveUmount();
						m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("Импорт данных"),
								M("Данные успешно импортированы"), "IMDB", Flags::GreenTitle | Flags::CloseButton,
								"icon_save_green.png"));
						LD("DB was imported from sql backup");
					} else if (ctx.m_dev_mon.getUsbDrives().back().isFileExisted("in.emi")) {
						ctx.m_dev_mon.getUsbDrives().back().driveMount();
						ctx.m_dev_mon.getUsbDrives().back().fileCopy("/media/usb/in.emi", "in.emi");
						ctx.m_db.importFromEmi("in.emi");
						ctx.m_dev_mon.getUsbDrives().back().driveUmount();
						m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("Импорт данных"),
								M("Данные успешно импортированы"), "IMDB", Flags::GreenTitle | Flags::CloseButton,
								"icon_save_green.png"));
						LD("DB was imported from emi file");
					} else {
						m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("Импорт данных"),
							M("Файл данных не найден"), "IMDB", Flags::YellowTitle | Flags::CloseButton,
							"icon_save_cross_yellow.png"));
					}
				}

				return nullptr;
			}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(920, 496, 213, 184, style, M("ИМПОРТ"))));

	style = default_style;
	style.font_size = 24;
	style.halign = Align::Left;
	style.text_color_disabled = to_base(Colors::Black);
	addWidget(std::make_shared<WidgetTextField>(138, 58, 992, 96, style))->setText(M("1. Подключите FLASH-накопитель к USB-разъему беговой дорожки"));
	addWidget(std::make_shared<WidgetTextField>(138, 292, 992, 204, style))->setText(
			M("2. Нажмите кнопку ЭКСПОРТ для передачи данных из\\"
			"   беговой дорожки во FLASH-накопитель.\\"
			"   Нажмите кнопку ИМПОРТ для передачи данных из\\"
			"   FLASH-накопителя в беговую дорожку."));

	style.halign = Align::Center;
	style.text_color_disabled = to_base(Colors::White);
	addWidget(std::make_shared<WidgetTextField>(159, 161, 473, 41, style))->setText(M("СТАТУС ПОДКЛЮЧЕНИЯ"));
	addWidget(std::make_shared<WidgetTextField>(648, 161, 473, 41, style))->setText(M("ОБЪЕМ СВОБОДНОЙ ПАМЯТИ"));

	style.text_color_disabled = to_base(Colors::BlueOnYellow);
	std::shared_ptr<WidgetTextField> connect_status;
	(connect_status = addWidget(std::make_shared<WidgetTextField>(159, 210, 489, 82, style)))->setText(M("FLASH-накопитель отключен"));
	std::shared_ptr<WidgetTextField> free_space;
	(free_space = addWidget(std::make_shared<WidgetTextField>(640, 210, 489, 82, style)))->setText("- - -  " + M("Kбайт"));

	if (ctx.m_dev_mon.getUsbDrives().size() == 0 || !ctx.m_dev_mon.getUsbDrives().back().isValid()) {
		m_export->setState(Widget::State::Disabled);
		m_import->setState(Widget::State::Disabled);
	} else {
		free_space->setText(std::to_string(ctx.m_dev_mon.getUsbDrives().back().getFreeSpace() / 1024) + " " + M("Кбайт"));
		connect_status->setText(M("FLASH-накопитель подключен"));
	}

	ctx.m_dev_mon.setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)> ([this, free_space, connect_status](auto s, auto u){
		if (s == "add") {
			m_export->setState(Widget::State::Passive);
			m_import->setState(Widget::State::Passive);
			free_space->setText(std::to_string(u.getFreeSpace() / 1024) + " " + M("Кбайт"));
			connect_status->setText(M("FLASH-накопитель подключен"));
		} else if (s == "remove") {
			m_export->setState(Widget::State::Disabled);
			m_import->setState(Widget::State::Disabled);
			free_space->setText("- - -  " + M("Kбайт"));
			connect_status->setText(M("FLASH-накопитель отключен"));
		}
	}));
}

SettingsExportMenu::~SettingsExportMenu() {
	ctx.m_dev_mon.setUsbDriveEventListener(std::function<void(std::string type, UsbFlashDrive &drive)>());
	LD("Dtor");
}

Menu* SettingsExportMenu::processEvent(std::unique_ptr<Event> event) {
	Menu *ret = nullptr;

	LD("Event enter " + (*event).getName());

	if (m_modal && ((*event).getType() == Event::EventType::ModalCancel || (*event).getType() == Event::EventType::ModalYes)) {
		if ((*m_modal).getType() == "EDEX") {
			if ((*event).getType() == Event::EventType::ModalYes) {
				m_modal.reset();
				doExport();
				LD("Do modal reset 1");
			} else if ((*event).getType() == Event::EventType::ModalCancel) {
				LD("Do modal reset 2");
				m_modal.reset();
			}
		} else if ((*m_modal).getType().substr(2, 2) == "DB"
				&& ((*event).getType() == Event::EventType::ModalCancel || (*event).getType() == Event::EventType::ModalYes)) {
			LD("Do modal reset 3");
			m_modal.reset();
		}
	} else {
		ret = Menu::processEvent(std::move(event));
	}

	LD("Event processed");
	return ret;
}

void SettingsExportMenu::doExport() {
	bool result = true;
	std::string db_file = ctx.m_db.backupDB();
	try {
		if (ctx.m_dev_mon.getUsbDrives().back().isValid()) {
			ctx.m_dev_mon.getUsbDrives().back().driveMount();
			ctx.m_dev_mon.getUsbDrives().back().fileCopy(db_file, "/media/usb/reaterra_db_backup.sql.gz");
			ctx.m_db.exportToEmi("em0.emi");
			ctx.m_dev_mon.getUsbDrives().back().fileCopy("em0.emi", "/media/usb/out.emi");
			ctx.m_dev_mon.getUsbDrives().back().fileCopy("history.db", "/media/usb/history.db");
			ctx.m_dev_mon.getUsbDrives().back().driveUmount();
			LD("DB was exported");
		} else {
			result = false;
		}
	} catch (...) {
		result = false;
	}

	using Flags = ModalCommonDialog::Flags;
	if (result) {
		LD("Files exported modal");
		m_modal = std::make_unique<ModalCommonDialog>(M("Экспорт данных"),
				M("Данные успешно экспортированы"), "EXDB", Flags::GreenTitle | Flags::CloseButton,
				"icon_save_green.png");
	} else {
		LD("Export failed modal");
		m_modal = std::make_unique<ModalCommonDialog>(M("Экспорт данных"),
			M("Ошибка экспорта данных!"), "IMDB", Flags::YellowTitle | Flags::CloseButton,
			"icon_save_cross_yellow.png");
	}
}
