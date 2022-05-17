#include "Menu.h"

#include <chrono>
#include <functional>

#include "Context.h"
#include "MenuEvent.h"
#include "ModalCommonDialog.h"
#include "ModalCommonError.h"
#include "TypeParser.h"
#include "WidgetImageButton.h"
#include "WidgetTextField.h"
#include "Utils.h"

#include "PatientListMenu.h"
#include "SettingsHrMenu.h"
#include "MainMenu.h"
#include "SasServiceMenu.h"


Menu::Menu(int x, int y, std::string background, WidgetSPVec &&widgets, std::string menu_type)
:	m_modal {nullptr},
	m_background_id {ctx.m_pic_mgr.loadPicture(background)},
	m_widgets (widgets),
	m_x {x},
	m_y {y},
	m_type {menu_type},
	m_check_events {true},
	m_width {0},
	m_height {0},
	m_event {EventType::None},
	m_clock_timer {[this](int id) {
			static int cnt = 0;
			if (++cnt > 10) {
				cnt = 0;
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
			}
		}, 0}
{
	LD("Ctor");

	m_prev_unl_warn = false;

	// Ждем когда появятся данные о конфигурации для настройки интерфейса.
	if (ctx.m_treadmill.getControllerVersion() > 0 && ctx.m_settings.getParamInt("check_connection") != 0) {
		for (int i = 0; i < 20 && !ctx.m_treadmill.isInitCompleted(); ++i) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			LD("Init waiting");
		}
	}

	int hr_x_offset = 0;

	if (!x && !y && m_background_id) {

		WTFStyle style {default_style};
		style.font_size = 22;
		style.font_style = FontStyle::Bold;
		style.text_color_disabled = 0x444444;
		style.halign = Align::Right;

		if (ctx.m_treadmill.isUnloadingExists() && getType() != "REXT") {
			hr_x_offset = 80;
			(m_unl_batt = addWidget(std::make_shared<WidgetImageButton>(1145, 752, "", "footer_battery.png")))->setState(Widget::State::Disabled);
			style.update_func = [](WidgetTextField &w) {
				int val = static_cast<int>(ctx.m_treadmill.getUnlBattLevel());
				std:: string str = ctx.m_treadmill.isUnlBattWarn() ? "- - -" : std::to_string(val);
				return str;
			};
			addWidget(std::make_shared<WidgetTextField>(1095, 769, 60, 31, style));
			addWidget(std::make_shared<WidgetImageButton>(1095, 746, "", "footer_separator.png"))->setState(Widget::State::Disabled);
		}

		addWidget(std::make_shared<WidgetImageButton>(1060 - hr_x_offset, 752, "", "footer_hr.png"))->setState(Widget::State::Disabled);
		addWidget(std::make_shared<WidgetImageButton>(135, 746, "", "footer_separator.png"))->setState(Widget::State::Disabled);
		addWidget(std::make_shared<WidgetImageButton>(919 - hr_x_offset, 746, "", "footer_separator.png"))->setState(Widget::State::Disabled);
		addWidget(std::make_shared<WidgetImageButton>(1171, 746, "", "footer_separator.png"))->setState(Widget::State::Disabled);

		style.update_func = [](WidgetTextField &w) {
			int hr = static_cast<int>(ctx.m_hr_monitor.getHRate());
			return hr ? std::to_string(hr) : "- - -";
		};
		addWidget(std::make_shared<WidgetTextField>(1077 - hr_x_offset, 769, 60, 31, style));
		style.update_func = [](WidgetTextField &w) {
			int bat = static_cast<int>(ctx.m_hr_monitor.getBattery());
			return bat < 0 ? "- - -" : std::to_string(bat);
		};
		addWidget(std::make_shared<WidgetTextField>(1009 - hr_x_offset, 769, 60, 31, style));
		style.halign = Align::Center;
		style.update_func = [](WidgetTextField &w) {
			return Utils::getCurrentTimeString("%H:%M");
		};
		addWidget(std::make_shared<WidgetTextField>(1175, 736, 102, 54, style));
		style.font_style = FontStyle::Normal;
		style.font_size = 16;
		style.update_func = [](WidgetTextField &w) {
			return Utils::getCurrentTimeString("%F");
		};
		addWidget(std::make_shared<WidgetTextField>(1175, 775, 102, 24, style));


		patient_footer_style.update_func = [this](WidgetTextField &w) {
			if (ctx.m_patient.empty()) {
				return M("ПАЦИЕНТ") + ": <" + M("не выбран") + ">";
			} else {
				return M("ПАЦИЕНТ") + ": " + ctx.m_patient;
			}
		};

		addWidget(std::make_shared<WidgetTextField>(250, 754, 590 - hr_x_offset, 39, patient_footer_style))->setText(M("ПАЦИЕНТ: ") + ctx.m_patient);

		m_patients_btn = addWidget(std::make_shared<WidgetImageButton>(147, 752, "footer_patients_btn_active.png", "footer_patients_btn_disabled.png", "footer_patients_btn_passive.png",
				[](WidgetImageButton::Event e) {
					return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());
				}, WidgetImageButton::Type::Clicker));

//		addWidget(std::make_shared<WidgetImageButton>(843, 752, "footer_settings_btn_active.png", "", "footer_settings_btn_passive.png",
//				[](WidgetImageButton::Event e) {
//					return std::make_unique<MenuEvent>(std::make_unique<PatientListMenu>());
//				}, WidgetImageButton::Type::Clicker));

		m_hr_btn = addWidget(std::make_shared<WidgetImageButton>(932 - hr_x_offset, 752, "footer_hr_btn_active.png", "footer_hr_btn_disabled.png", "footer_hr_btn_passive.png",
				[](WidgetImageButton::Event e) {
					return std::make_unique<MenuEvent>(std::make_unique<SettingsHrMenu>());
				}, WidgetImageButton::Type::Clicker));

		m_back_btn = addWidget(std::make_shared<WidgetImageButton>(3, 752, "back_active.png", "back_disabled.png", "back_passive.png",
				[this](WidgetImageButton::Event e) {

					std::unique_ptr<::Event> res = std::make_unique<MenuEvent>(std::unique_ptr<Menu>());
					if (m_on_back != nullptr) {
						LD("on_back != nullptr");
						res = m_on_back(e);
					}

					if (res.get() != nullptr) {
						LD("res != 0");

						std::unique_ptr<Menu> menu;
						ctx.m_menu_history.pop_back();
						if (ctx.m_menu_history.empty()) {
							menu = std::make_unique<MainMenu>();
						} else {
							menu = TypeParser::createObject(ctx.m_menu_history.back());
						}
						ctx.m_menu_history.pop_back();
						return std::make_unique<MenuEvent>(std::move(menu));
					} else {
						LD("res == 0");
						return std::make_unique<MenuEvent>(nullptr);
					}
				}, WidgetImageButton::Type::Clicker, std::make_shared<WidgetTextField>(12, 756, 111, 35, back_style)));
	}
	for (auto &w : widgets) {
		w->setParent(this);
	}
	//TODO: Перенести постоянные таймеры в контекст.
	//TODO: Сделать общий тред для таймеров - типа SWTimer
	m_clock_timer.startTimerMillis(100);
}

Menu::~Menu() {
	LD("Dtor");
}

Menu *Menu::processEvent(std::unique_ptr<Event> event) {
	Menu* ret = nullptr;

	if (!event) {
		return ret;
	}

	if (!m_sas_service_menu_counter && m_x == 0 && m_y == 0) {

		if (m_modal && m_modal->getType() != "SSSR") {
			m_modal.reset();
		}

		if (!m_modal) {
			m_modal = std::make_unique<SasServiceMenu>();
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}


		if (event->getType() == Event::EventType::Refresh) {
			show();
		}
		m_modal->processEvent(std::move(event));
		return ret;
	}

	if (!m_x && !m_y && m_background_id && ctx.m_treadmill.isUnloadingExists() && getType() != "REXT" && (ctx.m_treadmill.isUnlBattWarn() != m_prev_unl_warn)) {
		m_prev_unl_warn = ctx.m_treadmill.isUnlBattWarn();
		if (m_prev_unl_warn) {
			m_unl_batt->setDisabled("footer_battery_alarm.png");
		} else {
			m_unl_batt->setDisabled("footer_battery.png");
		}
	}

	if (m_modal && m_modal->getType() == "OVST" && event->getType() == Event::EventType::ModalYes) {
		LD("Reset cross error");
		ctx.m_treadmill.resetError();
	}

	if (m_modal && (m_modal->getType() == "SSER" || m_modal->getType() == "SSMS") && event->getType() == Event::EventType::IconClick) {
		--m_sas_service_menu_counter;
	}

	if (m_modal && m_modal->getType() == "SSTN" && event->getType() == Event::EventType::ModalYes) {
		LD("Start treadmill SAS test");
		ctx.m_treadmill.startSasTest();
		if (ctx.m_run_sas_test) {
			ctx.m_treadmill.setAcceleration(ctx.m_treadmill.getAccelTimeValue(7) * 10);
			ctx.m_treadmill.setDeceleration(ctx.m_treadmill.getAccelTimeValue(7) * 10);
			ctx.m_treadmill.setSpeed(ctx.m_treadmill.getMaxForwardSpeed());
			ctx.m_treadmill.setState(TreadmillDriver::RunState::Run);
		}
	}

	if (m_modal && m_modal->getType() == "FLHP" && event->getType() == Event::EventType::ModalYes) {
		LD("Reset fallen error");
		ctx.m_treadmill.resetFallenError();
		ctx.m_treadmill.resetFallenOnTreadError();
	}

	if (m_modal && m_modal->getType() == "OPTE" && event->getType() == Event::EventType::ModalYes) {
		LD("Reset optic error");
		ctx.m_treadmill.resetOpticError();
	}

	if (m_modal && m_modal->getType() == "PFER" && event->getType() == Event::EventType::ModalYes) {
		LD("Reset frequency converter error");
		ctx.m_treadmill.resetPfError();
	}

	if (m_modal && m_modal->getType() == "AOER" && event->getType() == Event::EventType::ModalYes) {
		LD("Reset actuator overload error");
		ctx.m_treadmill.resetActuatotError();
	}

	if (m_modal && (event->getType() == Event::EventType::ModalYes
			|| event->getType() == Event::EventType::ModalCancel
			|| event->getType() == Event::EventType::ModalNo))
	{
		m_modal.reset();
		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
	}

	if (event->getType() == Event::EventType::Refresh) {
		if (m_check_events) {
//			LD("Check");
			m_event = checkEvents();
		}

		show();
		return ret;
	}

	if (event->getType() == Event::EventType::Touch || event->getType() == Event::EventType::KbdEvent) {
		if (m_modal && ((m_modal->getType() != "KBDA" && m_modal->getType() != "KBDD") || event->getType() == Event::EventType::Touch)) {
			m_modal->processEvent(std::move(event));
		} else {

			if (event->getType() == Event::EventType::KbdEvent) {
				for (auto w : m_widgets) {
					if (w->m_kbd_focus) {
						w->processEvent(event.get());
						break;
					}
				}
			} else if (event->getType() == Event::EventType::Touch){
				ret = touchProcess(std::move(event));
			}
		}
	}

	return ret;
}

void Menu::show(bool final) {
	ctx.m_graphic.drawSurface(ctx.m_pic_mgr.getPicture(m_background_id), m_x, m_y);
//	LD(getType() + " Background show");

	for (auto &w : m_widgets) {
		w->show();
	}

	if (m_modal) {
//		LD(getType() + " Modal show");
		m_modal->show(false);
	}

	if (final) {
//		LD(getType() + " Final show");
		ctx.m_graphic.show();
	}
}

WidgetSP Menu::getSelected(int x, int y) {
	WidgetSP ret;

	for (auto w : m_widgets) {
		if (w->getState() == Widget::State::Disabled) {
			continue;
		}
		if (w->isSelected(x, y)) {
			ret = w;
			break;
		}
	}

	return ret;
}

int Menu::getX() const {
	return m_x;
}

void Menu::setX(int x) {
	m_x = x;
}

int Menu::getY() const {
	return m_y;
}

void Menu::setY(int y) {
	m_y = y;
}

const std::string& Menu::getType() const {
	return m_type;
}

void Menu::setType(const std::string& type) {
	m_type = type;
}

int Menu::getWidth() const {
	return m_width;
}

int Menu::getHeight() const {
	return m_height;
}

void Menu::setPatient(std::string name, int id, float weight, int profile_id) {
	ctx.m_patient = name;
	ctx.m_patient_id = id;
	ctx.m_patient_weight = weight;
	ctx.m_profile_id = profile_id;

	if (profile_id != 0) {
		ctx.m_prev_menu["RehabProfileRunMenu.params"] = {std::to_string(ctx.m_profile_id)};
	}
}

Menu::EventType Menu::checkEvents() {
	EventType ret = EventType::None;

	if (!ctx.m_treadmill.isPlcConnected() && ctx.m_settings.getParamInt("check_connection") != 0) {
		if (m_modal && m_modal->getType() != "PLCC") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			m_modal = std::make_unique<ModalCommonError>(M("ОШИБКА СВЯЗИ С\\КОНТРОЛЛЕРОМ"), M("ОБРАТИТЕСЬ В\\СЕРВИСНУЮ СЛУЖБУ"), "PLCC");
		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "PLCC") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isEmegencyStop()) {
		if (m_modal && m_modal->getType() != "EMRG") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			if (ctx.m_treadmill.isSasExists() && ctx.m_treadmill.isSasTestInitiated() && ctx.m_treadmill.isSasTestNeeded()) {
				ctx.m_treadmill.stopSasTest();
			}
			m_modal = std::make_unique<ModalCommonError>(M("ЭКСТРЕННАЯ ОСТАНОВКА\\ПОЛЬЗОВАТЕЛЕМ"), M("РАЗБЛОКИРУЙТЕ КНОПКУ"), "EMRG");
		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "EMRG") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isPFreqAlarm() && ctx.m_settings.getParamInt("check_connection") != 0) {
		if (m_modal && m_modal->getType() != "PFRQ") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			m_modal = std::make_unique<ModalCommonError>(M("ОШИБКА СВЯЗИ С\\ПРЕОБРАЗОВАТЕЛЕМ\\ЧАСТОТЫ"), M("ОБРАТИТЕСЬ В\\СЕРВИСНУЮ СЛУЖБУ"), "PFRQ");
		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "PFRQ") {
			m_modal.reset();
			Menu::processEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	}

	if (ctx.m_treadmill.isPFreqError()) {
		if (m_modal && m_modal->getType() != "PFER") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОШИБКА ПРЕОБРАЗОВАТЕЛЯ\\ЧАСТОТЫ"),
					"", "PFER", Flags::YellowTitle | Flags::CloseButton | Flags::StopIcon,
					"icon_treadmill_exclamation_yellow.png"));
			}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "PFER") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isActuatorOverloaded()) {
		if (m_modal && m_modal->getType() != "AOER") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ПЕРЕГРУЗКА АКТУАТОРА"),
					"", "AOER", Flags::YellowTitle | Flags::CloseButton | Flags::StopIcon,
					"icon_treadmill_exclamation_yellow.png"));
			}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "AOER") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isDriveOverheatError()) {
		if (m_modal && m_modal->getType() != "DOER") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			m_modal = std::make_unique<ModalCommonError>(M("ПЕРЕГРЕВ ДВИГАТЕЛЯ"), M("ОЖИДАЙТЕ ОСТЫВАНИЯ\\ДВИГАТЕЛЯ"), "DOER");
		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "DOER") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isSasConnectionError()) {
		if (m_modal && m_modal->getType() != "SASC") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			m_modal = std::make_unique<ModalCommonError>(M("ВНИМАНИЕ!"), M("ОШИБКА СВЯЗИ С АРС"), "SASC");
		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "SASC") {
			m_modal.reset();
		}
	}


	if (ctx.m_treadmill.isSasExists()) {

		if (!ctx.m_treadmill.isSasTestNeeded() && m_modal && !ctx.m_treadmill.isSasTestCompletedWithError()) {
			if (m_modal->getType() == "SSTS") {
				m_modal.reset();
			}

			if (!m_modal) {

				std::unique_ptr<ModalCommonDialog> no_hr_modal;
				using Flags = ModalCommonDialog::Flags;
				if (ctx.m_treadmill.isSasTestCompletedSuccessfull()) {
					no_hr_modal = std::make_unique<ModalCommonDialog>(M("САМОТЕСТИРОВАНИЕ\\БЕГОВОЙ ДОРОЖКИ"),
							M("ПРОЦЕДУРА ВЫПОЛНЕНА УСПЕШНО") , "SSMS", Flags::GreenTitle,
							"icon_treadmill_green.png");
					no_hr_modal->setTimer(1000, nullptr, 10, 4);
				} else {
					no_hr_modal = std::make_unique<ModalCommonDialog>(M("САМОТЕСТИРОВАНИЕ\\БЕГОВОЙ ДОРОЖКИ"),
							M("ПРОЦЕДУРА ЗАВЕРШЕНА.\\\\ФУНКЦИОНАЛЬНОСТЬ ОГРАНИЧЕНА.\\НЕДОСТУПНЫ РЕЖИМЫ АРС.") ,
							"SSMS", Flags::GreenTitle | Flags::IconClickable,
							"icon_treadmill_green.png");
					no_hr_modal->setTimer(1000, nullptr, 10, 6);
				}

				m_modal = std::move(no_hr_modal);
			}

			if (m_modal->getType() == "SSMS") {
				return ret;
			}
		}

		if (ctx.m_treadmill.isSasTestCompletedWithError()) {
			if (m_modal && m_modal->getType() != "SSER") {
				m_modal.reset();
			}
			if (!m_modal) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("НЕИСПРАВНОСТЬ\\БЕГОВОЙ ДОРОЖКИ"),
						M("ОБРАТИТЕСЬ В\\СЕРВИСНУЮ СЛУЖБУ"), "SSER", Flags::YellowTitle | Flags::IconClickable,
						"icon_treadmill_exclamation_yellow.png"));
				}
			return ret;
		} else {
			if (m_modal && m_modal->getType() == "SSER") {
				m_modal.reset();
			}
		}


	}



	if (ctx.m_treadmill.isFallHappen() || ctx.m_treadmill.isFallenOnTread()) {
		if (m_modal && m_modal->getType() != "FLHP") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Pause;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ПАДЕНИЕ НА ПОЛОТНО"),
					"", "FLHP", Flags::YellowTitle | Flags::CloseButton | Flags::StopIcon,
					"icon_treadmill_exclamation_yellow.png"));
			}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "FLHP") {
			m_modal.reset();
		}
	}

	if (ctx.m_treadmill.isOpticError()) {
		if (m_modal && m_modal->getType() != "OPTE") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Pause;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ОПТИЧЕСКАЯ ПОМЕХА"),
					"", "OPTE", Flags::YellowTitle | Flags::CloseButton | Flags::StopIcon,
					"icon_treadmill_exclamation_yellow.png"));
			}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "OPTE") {
			m_modal.reset();
		}
	}


	if (ctx.m_treadmill.isCrossError()) {
		if (m_modal && m_modal->getType() != "OVST") {
			m_modal.reset();
		}
		if (!m_modal) {
//			if (ctx.m_treadmill.getActualSpeed() < 500) {
//				ctx.m_treadmill.resetError();
//			} else {

			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Pause;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ЗАСТУП ЗА КРАЙ ПОЛОТНА"),
					"", "OVST", Flags::YellowTitle | Flags::CloseButton | Flags::StopIcon,
					"icon_overstep_yellow.png"));
			}
//		}
		return ret;
	} else {
		if (m_modal && m_modal->getType() == "OVST") {
			m_modal.reset();
		}
	}

//	LD("TRACE actual speed: " + std::to_string(ctx.m_treadmill.getActualSpeed()));
	if (!ctx.m_treadmill.isZeroPoint() && ctx.m_settings.getParamInt("check_connection") != 0) {
//		LD("TRACE in init");
		if (m_modal && m_modal->getType() != "INTM") {
			m_modal.reset();
		}
		if (!m_modal) {
			Menu::processEvent(std::make_unique<TouchEvent>(TouchEvent::TouchType::Clear, 0, 0));
			ret = EventType::Stop;
			using Flags = ModalCommonDialog::Flags;
			m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("ПОДОЖДИТЕ"),
					M("ИДЕТ ПОДГОТОВКА ДОРОЖКИ"), "INTM", Flags::GreenTitle | Flags::NoSound,
					"icon_treadmill_green.png"));
		}
		return ret;
	} else {
//		LD("TRACE in clear init");
		if (m_modal && m_modal->getType() == "INTM") {
			m_modal.reset();
		}
	}


	if (ctx.m_treadmill.isSasExists()) {
		if (ctx.m_treadmill.isSasTestNeeded() && !ctx.m_treadmill.isSasTestInitiated()) {
			if (m_modal && m_modal->getType() != "SSTN") {
				m_modal.reset();
			}
			if (!m_modal) {
				std::string msg = ctx.m_run_sas_test ?
						"ПОКИНЬТЕ ПОЛОТНО И НАЖМИТЕ\\КНОПКУ 'НАЧАТЬ'. БЕГОВОЕ\\ПОЛОТНО БУДЕТ ЗАПУЩЕНО." :
						"ПОКИНЬТЕ ПОЛОТНО\\И НАЖМИТЕ КНОПКУ 'НАЧАТЬ'.";
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("САМОТЕСТИРОВАНИЕ\\БЕГОВОЙ ДОРОЖКИ"),
						M(msg),
						"SSTN", Flags::GreenTitle | Flags::NoSound | Flags::EmptyButton,
						"icon_treadmill_green.png", "НАЧАТЬ"));
			}
			return ret;
		} else {
			if (m_modal && m_modal->getType() == "SSTN") {
				m_modal.reset();
			}
		}


		if (ctx.m_treadmill.isSasTestNeeded() && ctx.m_treadmill.isSasTestInitiated()) {
			if (m_modal && m_modal->getType() != "SSTS") {
				m_modal.reset();
			}
			if (!m_modal) {
				using Flags = ModalCommonDialog::Flags;
				m_modal = std::unique_ptr<Menu>(new ModalCommonDialog(M("САМОТЕСТИРОВАНИЕ\\БЕГОВОЙ ДОРОЖКИ"),
						M("ПРОЦЕДУРА ЗАПУЩЕНА.\\ПОЖАЛУЙСТА НЕ НАСТУПАЙТЕ\\НА БЕГОВОЕ ПОЛОТНО.\\ПРОЦЕДУРА ЗАЙМЕТ\\НЕСКОЛЬКО СЕКУНД."),
						"SSTS", Flags::GreenTitle | Flags::NoSound,
						"icon_treadmill_green.png"));
			}
			return ret;
		} else {
			if (m_modal && m_modal->getType() == "SSTS") {
				m_modal.reset();
			}
		}


	}

	return ret;
}


Menu* Menu::touchProcess(std::unique_ptr<Event> event) {
	Menu* ret = nullptr;
	auto tev =  dynamic_cast<TouchEvent*>(event.get());
//			LD(getType() + "  TouchEvent x: " + std::to_string(tev->getX()) + "    y: " + std::to_string(tev->getY()));

	if (m_active_widget.get()) {
		ret = m_active_widget->processEvent(event.get());
		if (tev->getType() != TouchEvent::TouchType::Move) {
			m_active_widget = nullptr;
			ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		}
	} else {

		WidgetSP w = getSelected(tev->getX() - m_x, tev->getY() - m_y);
		if (w.get() != nullptr) {
			if (tev->getType() == TouchEvent::TouchType::Touch) {
				m_active_widget = w;
				ret = w->processEvent(event.get());
				ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
			}
		}
	}

	return ret;
}


void Menu::addBreadCrumps(std::vector<WBCElement> &&bread_crumps) {
	addWidget(std::make_shared<WidgetBreadCrumbs>(66, 0, 963, 47, std::move(bread_crumps)));
}

void Menu::addOnBackAction(std::function<std::unique_ptr<::Event>(WidgetImageButton::Event)> on_back) {
	m_on_back = on_back;
}

