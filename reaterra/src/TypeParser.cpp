#include "TypeParser.h"
#include "Logger.h"

#include "FreeRunMenu.h"
#include "FreeRunMenuSas.h"
#include "MainMenu.h"
#include "PatientDetailsMenu.h"
#include "PatientEditMenu.h"
#include "PatientHistoryMenu.h"
#include "PatientListMenu.h"
#include "PatientNewMenu.h"
#include "PatientSearchMenu.h"
#include "PatientStatsMenu.h"
#include "RehabBosHrMinMaxMenu.h"
#include "RehabBosHrPowerMenu.h"
#include "RehabExternalMenu.h"
#include "RehabMenu.h"
#include "RehabProfileRunMenu.h"
#include "RehabProtocolSelectionMenu.h"
#include "RehabSelHrMenu.h"
#include "RehabSelMethodMenu.h"
#include "RehabSelProfileMenu.h"
#include "RehabTestProtocolMenu.h"
#include "RehabUserProfilSelectMenu.h"
#include "RehabUserProtocolsEditMenu.h"
#include "RehabUserProtocolsMenu.h"
#include "RehabUserProtocolsViewMenu.h"
#include "SettingsAccelMenu.h"
#include "SettingsCalibrateMenu.h"
#include "SettingsClockMenu.h"
#include "SettingsExportMenu.h"
#include "SettingsExtraCalibActuatorMenu.h"
#include "SettingsExtraCalibDriveMenu.h"
#include "SettingsHrMenu.h"
#include "SettingsMenu.h"
#include "SettingsPasswordMenu.h"
#include "SettingsRegistersMenu.h"
#include "SettingsUnitsMenu.h"

namespace TypeParser {

	std::unique_ptr<Menu> createObject(std::string type_name) {
		if (type_name == "FreeRunMenu") {
			return std::make_unique<FreeRunMenu>();
		} else if (type_name == "FreeRunMenuSas") {
			return std::make_unique<FreeRunMenuSas>();
		} else if (type_name == "MainMenu") {
			return std::make_unique<MainMenu>();
		} else if (type_name == "PatientDetailsMenu") {
			return std::make_unique<PatientDetailsMenu>();
		} else if (type_name == "PatientEditMenu") {
			return std::make_unique<PatientEditMenu>();
		} else if (type_name == "PatientHistoryMenu") {
			return std::make_unique<PatientHistoryMenu>();
		} else if (type_name == "PatientListMenu") {
			return std::make_unique<PatientListMenu>();
		} else if (type_name == "PatientNewMenu") {
			return std::make_unique<PatientNewMenu>();
		} else if (type_name == "PatientSearchMenu") {
			return std::make_unique<PatientSearchMenu>();
		} else if (type_name == "PatientStatsMenu") {
			return std::make_unique<PatientStatsMenu>();
		} else if (type_name == "RehabBosHrMinMaxMenu") {
			return std::make_unique<RehabBosHrMinMaxMenu>();
		} else if (type_name == "RehabBosHrPowerMenu") {
			return std::make_unique<RehabBosHrPowerMenu>();
		} else if (type_name == "RehabExternalMenu") {
			return std::make_unique<RehabExternalMenu>();
		} else if (type_name == "RehabMenu") {
			return std::make_unique<RehabMenu>();
		} else if (type_name == "RehabProfileRunMenu") {
			return std::make_unique<RehabProfileRunMenu>();
		} else if (type_name == "RehabProtocolSelectionMenu") {
			return std::make_unique<RehabProtocolSelectionMenu>();
		} else if (type_name == "RehabSelHrMenu") {
			return std::make_unique<RehabSelHrMenu>();
		} else if (type_name == "RehabSelMethodMenu") {
			return std::make_unique<RehabSelMethodMenu>();
		} else if (type_name == "RehabSelProfileMenu") {
			return std::make_unique<RehabSelProfileMenu>();
		} else if (type_name == "RehabTestProtocolMenu") {
			return std::make_unique<RehabTestProtocolMenu>();
		} else if (type_name == "RehabUserProtocolsEditMenu") {
			return std::make_unique<RehabUserProtocolsEditMenu>();
		} else if (type_name == "RehabUserProfilSelectMenu") {
			return std::make_unique<RehabUserProfilSelectMenu>();
		} else if (type_name == "RehabUserProtocolsMenu") {
			return std::make_unique<RehabUserProtocolsMenu>();
		} else if (type_name == "RehabUserProtocolsViewMenu") {
			return std::make_unique<RehabUserProtocolsViewMenu>();
		} else if (type_name == "SettingsAccelMenu") {
			return std::make_unique<SettingsAccelMenu>();
		} else if (type_name == "SettingsCalibrateMenu") {
			return std::make_unique<SettingsCalibrateMenu>();
		} else if (type_name == "SettingsClockMenu") {
			return std::make_unique<SettingsClockMenu>();
		} else if (type_name == "SettingsExportMenu") {
			return std::make_unique<SettingsExportMenu>();
		} else if (type_name == "SettingsExtraCalibActuatorMenu") {
			return std::make_unique<SettingsExtraCalibActuatorMenu>();
		} else if (type_name == "SettingsExtraCalibDriveMenu") {
			return std::make_unique<SettingsExtraCalibDriveMenu>();
		} else if (type_name == "SettingsHrMenu") {
			return std::make_unique<SettingsHrMenu>();
		} else if (type_name == "SettingsMenu") {
			return std::make_unique<SettingsMenu>();
		} else if (type_name == "SettingsPasswordMenu") {
			return std::make_unique<SettingsPasswordMenu>();
		} else if (type_name == "SettingsRegistersMenu") {
			return std::make_unique<SettingsRegistersMenu>();
		} else if (type_name == "SettingsUnitsMenu") {
			return std::make_unique<SettingsUnitsMenu>();
		}

		LG("Type not found: " + type_name);
		return nullptr;
	}

}

