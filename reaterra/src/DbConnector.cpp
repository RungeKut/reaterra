#include "DbConnector.h"

#include <locale>
#include <sqlite3.h>
#include <string>

#include "Context.h"
#include "EnumExtension.h"
#include "Utils.h"

DbConnector::DbConnector()
:	m_init_ok {false}
{
	LD("Ctor");

	m_mysql = mysql_init(NULL);
	bool val = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &val);

	for (int i = 0; i < 30; ++i) {
		if (!mysql_real_connect(m_mysql, "localhost", "reauser", "rEa2AeR", "REATERRA", 0, NULL, 0)) {
			LE(mysql_error(m_mysql));
		} else {
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}


	command("SET CHARSET utf8");
	upgrade();
    m_init_ok = true;
}

DbConnector::~DbConnector() {
	LD("Dtor");
}

DataSet DbConnector::getProtocols(std::string type) {
	return select(R"SQL(

SELECT t1.`Название`, t1.`id`, t1.`Описание`
FROM `REATERRA`.`Протоколы` t1
INNER JOIN
(
    SELECT `Название`, MIN(`Порядок`) AS min_order
    FROM `REATERRA`.`Протоколы`
    WHERE `Тип` = ')SQL" + type + R"SQL('
    GROUP BY `Название`
) t2
    ON t1.`Название` = t2.`Название` AND
       t1.`Порядок` = t2.min_order
       AND `Тип` = ')SQL" + type + R"SQL('
 WHERE t1.id > 0
)SQL");
}

DataSet DbConnector::getProtocolModifs(int id) {
	return select(R"SQL(

SELECT t1.`Модификация`, t1.`id` FROM `REATERRA`.`Протоколы` t1
INNER JOIN
(
    SELECT `Название`, `Тип`
    FROM `REATERRA`.`Протоколы`
    WHERE `id` = )SQL" + std::to_string(id) + R"SQL(
) t2
    ON t1.`Название` = t2.`Название` AND
       t1.`Тип` = t2.`Тип`
ORDER BY `Порядок`

)SQL");
}

DataSet DbConnector::getIntervals(int id) {
	return select(R"SQL(

SELECT 	`Интервал`, 
		`Длительность`, 
		`Скорость`, 
		`Угол`, 
		`Тип`, 
		`Ускорение` 
FROM `REATERRA`.`Интервалы` 
WHERE `idПротокола` = )SQL" + std::to_string(id) + R"SQL(
ORDER BY `Порядок`

)SQL");
}

DataSet DbConnector::getProtocolParams(int id) {
	return select(R"SQL(

SELECT 	`Тип`, 
		`Название`, 
		`Модификация`, 
		`ДлительностьЕд`, 
		`ДлительностьФмт`, 
		`СкоростьЕд`, 
		`СкоростьФмт`, 
		`УголЕд`, 
		`УголФмт`, 
		`Описание`
FROM `REATERRA`.`Протоколы` 
WHERE `id` = )SQL" + std::to_string(id) + R"SQL(

)SQL");
}

std::string DbConnector::getParam(std::string name, std::string def) {
	if (name.empty()) {
		throw std::invalid_argument("Empty parameter name");
	}
	DataSet data = select("SELECT `Значение` FROM `REATERRA`.`Параметры` WHERE `Имя` = '" + name + "'");
	if (!data.size() || !data.at(0).size()) {
		LE("Error getting parameter [" + name + "] from database");
		if (def != "no_insert") {
			LD("Try to insert parameter to DB");
			command("INSERT INTO `REATERRA`.`Параметры` (`Имя`, `Значение`) " + std::string{} + "VALUES ('" + name + "', '" + def + "');");
			return def;
		} else {
			return "";
		}
	} else {
		return data.at(0).at(0);
	}
}

void DbConnector::setParam(std::string name, std::string value) {
	command("UPDATE `REATERRA`.`Параметры` SET `Значение`='" + value + "' WHERE `Имя`='" + name + "'");
}

DataSet DbConnector::select(std::string query) {
    command(query);

    DataSet ret;
    if (MYSQL_RES *result = mysql_store_result(m_mysql)) {
    	if (int num_of_fields = mysql_num_fields(result)) {
			while (MYSQL_ROW row = mysql_fetch_row(result)) {
				ret.push_back(std::vector<std::string>{});
				for (int i = 0; i < num_of_fields; ++i) {
					ret.back().push_back(std::string(row[i] ? Utils::allTrim(row[i]) : ""));
				}
			}
    	}

		mysql_free_result(result);
    } else {
        LE(mysql_error(m_mysql));
        throw DbError(mysql_error(m_mysql), mysql_errno(m_mysql));
    }

    return ret;
}

std::string DbConnector::backupDB() {
	std::string backup_file {"reaterra_db_dump_" + Utils::getFileTimestamp() + ".sql.gz"};
	int ret = system(("mysqldump REATERRA -ureauser -prEa2AeR | gzip > " + backup_file).c_str());
	if (ret) {
		LE("Database backup error");
		throw std::runtime_error("Database backup error");
	} else {
		LD("DB was dumped");
	}

	return backup_file;
}

void DbConnector::restoreDB(std::string gz_dump) {
	int ret = system(("gzip -d -c " + gz_dump + " | mysql -u reauser -prEa2AeR REATERRA").c_str());
	if (ret) {
		LE("Database restore error");
		throw std::runtime_error("Database restore error");
	} else {
		LD("DB was restored");
	}
}

Utils::Unit DbConnector::getParamEnum(std::string name) {
	Utils::Unit ret = Utils::Unit::Unknown;

	std::string unit = getParam(name);

	if (unit == "км") {
		ret = Utils::Unit::KiloMeter;
	} else if (unit == "м") {
		ret = Utils::Unit::Meter;
	} else if (unit == "мили") {
		ret = Utils::Unit::Mile;
	} else if (unit == "км/ч") {
		ret = Utils::Unit::KmPerHour;
	} else if (unit == "м/с") {
		ret = Utils::Unit::MPerSec;
	} else if (unit == "миля/ч") {
		ret = Utils::Unit::MilePerHour;
	} else if (unit == "град") {
		ret = Utils::Unit::Degree;
	} else if (unit == "%") {
		ret = Utils::Unit::Procent;
	} else if (unit == "ккал") {
		ret = Utils::Unit::KiloKall;
	} else if (unit == "Дж") {
		ret = Utils::Unit::Joul;
	} else if (unit == "кВт·ч") {
		ret = Utils::Unit::KWattPerHour;
	} else if (unit == "Вт") {
		ret = Utils::Unit::Watt;
	} else if (unit == "кВт") {
		ret = Utils::Unit::KiloWatt;
	} else if (unit == "МЕТ") {
		ret = Utils::Unit::MET;
	} else if (unit == "мл/мин/кг") {
		ret = Utils::Unit::MlMinKg;
	}

	return ret;
}

DataSet DbConnector::getPatients(std::string order, bool asc, std::string filter) {
	std::string where;

	if (!filter.empty()) {
		where = "WHERE u.`Фамилия` COLLATE UTF8MB4_UNICODE_CI LIKE '" + filter + "%'";
	}

	return select(R"SQL(

SELECT 	u.`id`, 
		u.`Номер`, 
		u.`Фамилия`, 
		u.`Имя`, 
		u.`Отчество`, 
		u.`Пол`, 
		u.`Возраст`, 
		u.`Диагноз`, 
		u.`Врач`, 
		u.`Вес`, 
		u.`Профиль`, 
		p.`Название` 
FROM `REATERRA`.`Пациенты` AS u
LEFT JOIN `REATERRA`.`Протоколы` AS p 
	ON u.`Профиль` = p.`id` 
)SQL" + where + R"SQL( 
ORDER BY u.`)SQL" + order + "` " + (asc ? "ASC" : "DESC") + R"SQL(

)SQL");
}

void DbConnector::deletePatient(int id) {
	command(R"SQL(

DELETE FROM `REATERRA`.`Пациенты` 
WHERE `id` = )SQL" + std::to_string(id) + R"SQL(

)SQL");


	command(R"SQL(

UPDATE `REATERRA`.`Пациенты`
JOIN (SELECT @rank := 0) r 
SET `Номер` = @rank := @rank + 1

)SQL");


	command(R"SQL(

DELETE FROM `REATERRA`.`История`
WHERE `Пациент` = )SQL" + std::to_string(id) + R"SQL(

)SQL");

}

DataSet DbConnector::getPatient(int id) {
	if (id < 0) {
		return select(R"SQL(

SELECT 	'-1', 
		MAX(`Номер`) + 1,
		'',   
		'',         
		'',   
		'М',       
		'',        
		'',     
		'',   
		'', 
		'', 
		'' 
FROM `REATERRA`.`Пациенты`

)SQL");

	} else {
		return select(R"SQL(

SELECT 	u.`id`, 
		u.`Номер`, 
		u.`Фамилия`, 
		u.`Имя`, 
		u.`Отчество`, 
		u.`Пол`, 
		u.`Возраст`, 
		u.`Диагноз`, 
		u.`Врач`, 
		u.`Вес`, 
		u.`Профиль`, 
		p.`Название` 
FROM `REATERRA`.`Пациенты` AS u 
LEFT JOIN `REATERRA`.`Протоколы` AS p 
	ON u.`Профиль` = p.`id` 
WHERE u.`id` = )SQL" + std::to_string(id) + R"SQL(

)SQL");
	}
}

int DbConnector::insertPatient(DataSet patient) {
	std::string last_name {patient.at(0).at(to_base(PatientList::LastName))};
	std::string first_name {patient.at(0).at(to_base(PatientList::FirstName))};
	std::string middle_name {patient.at(0).at(to_base(PatientList::MiddleName))};
	DataSet data = select(R"SQL(

SELECT `id`
FROM `REATERRA`.`Пациенты`
WHERE 	`Фамилия` = ')SQL" + last_name + R"SQL('
	AND `Имя` = ')SQL" + first_name + R"SQL('
	AND `Отчество` = ')SQL" + middle_name + R"SQL('

)SQL");

	if (data.size()) {
		return -1;
	}

	std::string age {patient.at(0).at(to_base(PatientList::Age))};
	std::string diagnosis {patient.at(0).at(to_base(PatientList::Diagnosis))};
	std::string doctor {patient.at(0).at(to_base(PatientList::Doctor))};
	std::string sex {patient.at(0).at(to_base(PatientList::Sex))};
	std::string weight {patient.at(0).at(to_base(PatientList::Weight))};
	std::string profile {patient.at(0).at(to_base(PatientList::ProfileId))};

	if (profile == "") {
		profile = "0";
	}

	command("INSERT INTO `REATERRA`.`Пациенты` ( `Фамилия`,     `Имя`,             `Отчество`,          `Пол`,     `Возраст`,        `Диагноз`,           `Врач`,           `Вес`,           `Профиль`) " + std::string{} +
			                  "VALUES ('" + last_name + "', '" + first_name + "', '"+ middle_name +"', '"+ sex +"', '" + age + "', '" + diagnosis + "', '" + doctor + "', '" + weight + "', '" + profile + "');");
	data = select("SELECT LAST_INSERT_ID();");
	int protocol_id = std::stoi(data.at(0).at(0));

	command("UPDATE `REATERRA`.`Пациенты` JOIN (SELECT @rank := 0) r SET `Номер` = @rank := @rank + 1");

	return protocol_id;
}

void DbConnector::updatePatient(DataSet patient) {
	std::string id {patient.at(0).at(to_base(PatientList::Id))};
	std::string last_name {patient.at(0).at(to_base(PatientList::LastName))};
	std::string first_name {patient.at(0).at(to_base(PatientList::FirstName))};
	std::string middle_name {patient.at(0).at(to_base(PatientList::MiddleName))};
	DataSet data = select("SELECT `id` FROM `REATERRA`.`Пациенты` WHERE `Фамилия` = '"
			+ last_name + "' AND `Имя` = '" + first_name + "' AND `Отчество` = '" + middle_name + "' AND `id` <> '" + id + "'");
	if (data.size()) {
		return;
	}

	std::string age {patient.at(0).at(to_base(PatientList::Age))};
	std::string diagnosis {patient.at(0).at(to_base(PatientList::Diagnosis))};
	std::string doctor {patient.at(0).at(to_base(PatientList::Doctor))};
	std::string sex {patient.at(0).at(to_base(PatientList::Sex))};
	std::string weight {patient.at(0).at(to_base(PatientList::Weight))};
	std::string profile {patient.at(0).at(to_base(PatientList::ProfileId))};

	command("UPDATE `REATERRA`.`Пациенты` SET " + std::string{} +
			"`Фамилия` = '" + last_name + "', " +
			"`Имя` = '" + first_name + "', " +
			"`Отчество` = '" + middle_name + "', " +
			"`Пол` = '" + sex + "', " +
			"`Возраст` = '" + age + "', " +
			"`Диагноз` = '" + diagnosis + "', " +
			"`Врач` = '" + doctor + "', " +
			"`Вес` = '" + weight + "', " +
			"`Профиль` = '" + profile + "' " +
			" WHERE `id` = '" + id + "'");
}

std::string DbConnector::dumpDataSet(DataSet ds, std::string name) {
	std::string ret {name + "\n"};

	for (const auto &d : ds) {
		for (const auto &r : d) {
			ret += r + "\t";
		}
		ret += "\n";
	}

	return ret;
}

void DbConnector::addRunSession(RunSession& session) {
	DataSet patient = getPatient(session.getPatientId());

	command(R"SQL(

INSERT INTO `REATERRA`.`История` (
	`Пациент`,
	`Длительность`,
	`Расстояние`,
	`Скорость`,
	`Ускорение`,
	`Угол`,
	`Пульс`,
	`ПульсМин`,
	`ПульсМакс`,
	`Энергия`,
	`Мощность`,
	`МощностьАэробная`,
	`Профиль`,
	`ДлинаШага`,
	`СимметрияЛевая`,
	`СимметрияПравая`,
	`РежимСАС`
) VALUES (
	')SQL" + std::to_string(session.getPatientId()) + R"SQL(',
	')SQL" + std::to_string(session.getDuration()) + R"SQL(',
	')SQL" + std::to_string(session.getDistance()) + R"SQL(',
	')SQL" + std::to_string(session.getSpeedAvg()) + R"SQL(',
	')SQL" + std::to_string(session.getAccelMax()) + R"SQL(',
	')SQL" + std::to_string(session.getAngleMax()) + R"SQL(',
	)SQL" + (session.getHrateAvg() ? std::to_string(session.getHrateAvg()) : "NULL") + R"SQL(,
	)SQL" + (session.getHrateMin() ? std::to_string(session.getHrateMin()) : "NULL") + R"SQL(,
	)SQL" + (session.getHrateMax() ? std::to_string(session.getHrateMax()) : "NULL") + R"SQL(,
	')SQL" + std::to_string(session.getEnergy()) + R"SQL(',
	')SQL" + std::to_string(session.getPowerMechAvg()) + R"SQL(',
	')SQL" + std::to_string(session.getPowerAeroAvg()) + R"SQL(',
	')SQL" + session.getProfile() + R"SQL(',
	)SQL" + (session.getSasStepLengthAvg() ? std::to_string(session.getSasStepLengthAvg()) : "NULL") + R"SQL(,
	)SQL" + (session.getSasStepLengthAvg() ? std::to_string(session.getSasSymmetryLeftAvg()) : "NULL") + R"SQL(,
	)SQL" + (session.getSasStepLengthAvg() ? std::to_string(session.getSasSymmetryRightAvg()) : "NULL") + R"SQL(,
	')SQL" + session.getSasMode() + R"SQL('
)	
)SQL");
}

DataSet DbConnector::getStatistic(int patient_id) {
	return select(R"SQL(

SELECT 	CONCAT(
			u.`Фамилия`, 
			CASE WHEN u.`Имя` <> '' 
				THEN CONCAT(' ', SUBSTR(u.`Имя`, 1, 1), '.',
					CASE WHEN u.`Отчество` <> ''
						THEN CONCAT(SUBSTR(u.`Отчество`, 1, 1), '.')
						ELSE ''
					END) 
				ELSE '' 
			END), 
	SUM(h.`Расстояние`), 
	SUM(h.`Длительность`), 
	SUM(h.`Энергия`), 
	MAX(h.`Ускорение`), 
	MIN(h.`ПульсМин`), 
	MAX(h.`ПульсМакс`) 
FROM `REATERRA`.`История` AS h 
INNER JOIN `REATERRA`.`Пациенты` AS u 
ON h.`Пациент` = u.`id` 
WHERE u.`id` = )SQL" + std::to_string(patient_id) + R"SQL(

)SQL");
}

DataSet DbConnector::getHistory(std::string from, std::string to, std::string fio) {
	std::string where;

	if (!fio.empty()) {
		where = " u.`Фамилия` COLLATE UTF8MB4_UNICODE_CI LIKE '" + fio + "%'";
	}
	if (!from.empty()) {
		if (!where.empty()) {
			where += " AND";
		}
		where += " h.`Время` >= '" + from + " 00:00:00'";
	}
	if (!to.empty()) {
		if (!where.empty()) {
			where += " AND";
		}
		where += " h.`Время` <= '" + to + " 23:59:59'";
	}

	if (!where.empty()) {
		where = "WHERE" + where;
	}

	return select(R"SQL(

SELECT 	CONCAT(
			u.`Фамилия`, 
			CASE WHEN u.`Имя` <> '' 
				THEN CONCAT(' ', SUBSTR(u.`Имя`, 1, 1), '.',
					CASE WHEN u.`Отчество` <> ''
						THEN CONCAT(SUBSTR(u.`Отчество`, 1, 1), '.')
						ELSE ''
					END) 
				ELSE '' 
			END), 
	h.`Время`,
	h.`Профиль`, 
	h.`Расстояние`, 
	h.`Длительность`, 
	h.`Скорость`, 
	h.`Ускорение`, 
	h.`Угол`, 
	h.`Энергия`, 
	h.`Мощность`, 
	h.`МощностьАэробная`, 
	h.`Пульс`,
 	u.`Фамилия`,
	u.`Имя`,
	u.`Отчество`,
	UNIX_TIMESTAMP(h.`Время`),
	h.`ДлинаШага`,
	h.`СимметрияЛевая`,
	h.`СимметрияПравая`,
	h.`РежимСАС`
FROM `REATERRA`.`История` AS h 
INNER JOIN `REATERRA`.`Пациенты` AS u 
ON h.`Пациент` = u.`id` 
)SQL" + where + R"SQL( 

)SQL");
}

void DbConnector::command(std::string cmd) {
	if (ctx.m_settings.getParamInt("log_db_requests")) {
		LD(cmd);
//		LD(Utils::bytes2Hex((unsigned char *)(cmd.data()), cmd.size()));
	}
	if(mysql_ping(m_mysql)) {
        LE(mysql_error(m_mysql));
        throw DbError(mysql_error(m_mysql), mysql_errno(m_mysql));
	}

    if (mysql_query(m_mysql, cmd.c_str())) {
        LE(mysql_error(m_mysql));
        throw DbError(mysql_error(m_mysql), mysql_errno(m_mysql));
    }
}

int DbConnector::insertProtocol(DataSet protocol, DataSet intervals, std::string pro_type) {
	DataSet data = select("SELECT `id` FROM `REATERRA`.`Протоколы` WHERE `id` >= 0 AND `Название` = '" + protocol.at(0).at(to_base(ProtocolParam::Name)) + "'");
	if (data.size()) {
		return -1;
	}

	int max_order = 0;
	data = select("SELECT MAX(`Порядок`) FROM `REATERRA`.`Протоколы` WHERE `Тип` = 'USER'");
	if (data.size()) {
		max_order = std::stoi(data.at(0).at(0));
	}
	std::string order {std::to_string(max_order + 1)};
	std::string name {protocol.at(0).at(to_base(ProtocolParam::Name))};
	std::string descr {protocol.at(0).at(to_base(ProtocolParam::Description))};
	std::string modif {protocol.at(0).at(to_base(ProtocolParam::Modification))};
	std::string dur_u {protocol.at(0).at(to_base(ProtocolParam::DurationUnit))};
	std::string dur_f {protocol.at(0).at(to_base(ProtocolParam::DurationFmt))};
	std::string speed_u {protocol.at(0).at(to_base(ProtocolParam::SpeedUnit))};
	std::string speed_f {protocol.at(0).at(to_base(ProtocolParam::SpeedFmt))};
	std::string angle_u {protocol.at(0).at(to_base(ProtocolParam::AngleUnit))};
	std::string angle_f {protocol.at(0).at(to_base(ProtocolParam::AngleFmt))};

	command("INSERT INTO `REATERRA`.`Протоколы` ( `Тип`,    `Порядок`,    `Название`,     `Описание`,   `Модификация`, `ДлительностьЕд`, `ДлительностьФмт`, `СкоростьЕд`, `СкоростьФмт`,       `УголЕд`,      `УголФмт`) " + std::string{} +
			                  "VALUES ('" + pro_type + "', " + order + ", '"+ name +"', '"+ descr +"', '" + modif + "', '" + dur_u + "', '" + dur_f + "', '" + speed_u + "', '" + speed_f + "', '" + angle_u + "', '" + angle_f + "');");
	data = select("SELECT LAST_INSERT_ID();");
	int protocol_id = std::stoi(data.at(0).at(0));

	for (unsigned i = 0; i < intervals.size(); ++i) {
		std::string duration {intervals.at(i).at(to_base(IntervalData::Duration))};
		std::string angle {intervals.at(i).at(to_base(IntervalData::Angle))};
		std::string interval {intervals.at(i).at(to_base(IntervalData::Name))};
		std::string speed {intervals.at(i).at(to_base(IntervalData::Speed))};
		std::string type {intervals.at(i).at(to_base(IntervalData::Type))};
		std::string accel {intervals.at(i).at(to_base(IntervalData::Acceleration))};
		std::string cnt {std::to_string(i + 1)};

		if (accel.empty()) {
			accel = "1";
		}
		command("INSERT INTO `REATERRA`.`Интервалы` (  `Интервал`, `Порядок`,  `Длительность`,  `Скорость`,       `Угол`,        `Тип`,    `Ускорение`,         `idПротокола`) " + std::string{} +
				                            "VALUES (" + cnt + ", "+ cnt +", "+ duration +", " + speed + ", " + angle + ", " + type + ", " + accel + ", " + std::to_string(protocol_id) + ");");
	}

	return protocol_id;
}

void DbConnector::updateProtocol(int id, DataSet protocol, DataSet intervals) {
	std::string name {protocol.at(0).at(to_base(ProtocolParam::Name))};
	std::string descr {protocol.at(0).at(to_base(ProtocolParam::Description))};
	std::string modif {protocol.at(0).at(to_base(ProtocolParam::Modification))};
	std::string dur_u {protocol.at(0).at(to_base(ProtocolParam::DurationUnit))};
	std::string dur_f {protocol.at(0).at(to_base(ProtocolParam::DurationFmt))};
	std::string speed_u {protocol.at(0).at(to_base(ProtocolParam::SpeedUnit))};
	std::string speed_f {protocol.at(0).at(to_base(ProtocolParam::SpeedFmt))};
	std::string angle_u {protocol.at(0).at(to_base(ProtocolParam::AngleUnit))};
	std::string angle_f {protocol.at(0).at(to_base(ProtocolParam::AngleFmt))};
	command("UPDATE `REATERRA`.`Протоколы` SET " + std::string{} +
			"`Название` = '" + protocol.at(0).at(to_base(ProtocolParam::Name)) + "', " +
			"`Описание` = '" + protocol.at(0).at(to_base(ProtocolParam::Description)) + "', " +
			"`Модификация` = '" + protocol.at(0).at(to_base(ProtocolParam::Modification)) + "', " +
			"`ДлительностьЕд` = '" + protocol.at(0).at(to_base(ProtocolParam::DurationUnit)) + "', " +
			"`ДлительностьФмт` = '" + protocol.at(0).at(to_base(ProtocolParam::DurationFmt)) + "', " +
			"`СкоростьЕд` = '" + protocol.at(0).at(to_base(ProtocolParam::SpeedUnit)) + "', " +
			"`СкоростьФмт` = '" + protocol.at(0).at(to_base(ProtocolParam::SpeedFmt)) + "', " +
			"`УголЕд` = '" + protocol.at(0).at(to_base(ProtocolParam::AngleUnit)) + "', " +
			"`УголФмт` = '" + protocol.at(0).at(to_base(ProtocolParam::AngleFmt)) + "' " +
			" WHERE `id`=" + std::to_string(id));
	command("DELETE FROM `REATERRA`.`Интервалы` WHERE `idПротокола` = " + std::to_string(id));
	for (unsigned i = 0; i < intervals.size(); ++i) {
		std::string duration {intervals.at(i).at(to_base(IntervalData::Duration))};
		std::string angle {intervals.at(i).at(to_base(IntervalData::Angle))};
		std::string interval {intervals.at(i).at(to_base(IntervalData::Name))};
		std::string speed {intervals.at(i).at(to_base(IntervalData::Speed))};
		std::string type {intervals.at(i).at(to_base(IntervalData::Type))};
		std::string accel {intervals.at(i).at(to_base(IntervalData::Acceleration))};
		std::string cnt {std::to_string(i + 1)};
		if (accel.empty()) {
			accel = "1";
		}
		command("INSERT INTO `REATERRA`.`Интервалы` (  `Интервал`, `Порядок`,  `Длительность`,  `Скорость`,       `Угол`,        `Тип`,      `Ускорение`,        `idПротокола`) " + std::string{} +
				                            "VALUES (" + cnt + ", "+ cnt +", "+ duration +", " + speed + ", " + angle + ", " + type + ", " + accel + ", " + std::to_string(id) + ");");
	}
}

void DbConnector::deleteProtocol(int id) {
	command("DELETE FROM `REATERRA`.`Интервалы` WHERE `idПротокола` = " + std::to_string(id));
	command("DELETE FROM `REATERRA`.`Протоколы` WHERE `id` = " + std::to_string(id));
	command("UPDATE `REATERRA`.`Пациенты` SET `Профиль` = 0 WHERE `Профиль` = " + std::to_string(id));
}

void DbConnector::exportToEmi(std::string file) {
	DataSet patients {ctx.m_db.getPatients("Номер", true)};

	std::ofstream fout;
	fout.open(file, std::ios::binary | std::ios::out);
	unsigned char buff[288];

	for (const auto &r : patients) {
		int ind = 0;
		memset(buff, 0, sizeof(buff));
		buff[ind] = static_cast<uint16_t>(std::stoi(r.at(to_base(PatientList::Number))));
		ind += 2;
		std::u16string ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::LastName)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(40u, ws.size() * 2));
		ind += 40;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::FirstName)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(40u, ws.size() * 2));
		ind += 40;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::MiddleName)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(40u, ws.size() * 2));
		ind += 40;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::Sex)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), 2);
		ind += 2;
		buff[ind] = static_cast<uint16_t>(std::stoi(r.at(to_base(PatientList::Age))));
		ind += 2;
		buff[ind] = static_cast<uint16_t>(std::stoi(r.at(to_base(PatientList::Weight))));
		ind += 2;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::Diagnosis)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(80u, ws.size() * 2));
		ind += 80;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::Doctor)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(40u, ws.size() * 2));
		ind += 40;
		ws = Utils::utf8_to_u16string(r.at(to_base(PatientList::Profile)));
		memcpy(buff + ind, reinterpret_cast<const void*>(ws.data()), std::min(40u, ws.size() * 2));
		ind += 40;

		fout.write(reinterpret_cast<char*>(buff), sizeof(buff));
	}

	fout.close();

	sqlite3 *db = NULL;
	char dbPath[] = "history.db";
	int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        LD("Cannot open sqlite database: " + sqlite3_errmsg(db));
    } else {
		const char sql[] =
			"PRAGMA foreign_keys=OFF;"
			"DROP TABLE IF EXISTS data;"
			"DROP TABLE IF EXISTS data_format;"
			"DROP TABLE IF EXISTS '.data_section';"
			"CREATE TABLE IF NOT EXISTS data ("
				"data_index INTEGER PRIMARY KEY AUTOINCREMENT, 'time@timestamp' REAL, "
				"data_format_0 BLOB, data_format_1 BLOB, data_format_2 BLOB, data_format_3 BLOB, data_format_4 INTEGER, "
				"data_format_5 INTEGER, data_format_6 INTEGER, data_format_7 REAL, data_format_8 REAL, data_format_9 REAL, "
				"data_format_10 REAL, data_format_11 INTEGER, data_format_12 REAL, data_format_13 REAL, data_format_14 REAL);"
			"CREATE TABLE IF NOT EXISTS data_format (data_format_index INTEGER PRIMARY KEY, comment TEXT, data_type TEXT, size INTEGER);"
			"INSERT INTO data_format VALUES(0,'Family','STRING',20);"
			"INSERT INTO data_format VALUES(1,'Name','STRING',20);"
			"INSERT INTO data_format VALUES(2,'Name2','STRING',20);"
			"INSERT INTO data_format VALUES(3,'Profile','STRING',20);"
			"INSERT INTO data_format VALUES(4,'Hours','UINT16',1);"
			"INSERT INTO data_format VALUES(5,'Minute','UINT16',1);"
			"INSERT INTO data_format VALUES(6,'Second','UINT16',1);"
			"INSERT INTO data_format VALUES(7,'Distance','REAL32',1);"
			"INSERT INTO data_format VALUES(8,'Speed','REAL32',1);"
			"INSERT INTO data_format VALUES(9,'Acceleration','REAL32',1);"
			"INSERT INTO data_format VALUES(10,'Angle','REAL32',1);"
			"INSERT INTO data_format VALUES(11,'Pulse','UINT16',1);"
			"INSERT INTO data_format VALUES(12,'Energy','REAL32',1);"
			"INSERT INTO data_format VALUES(13,'Power','REAL32',1);"
			"INSERT INTO data_format VALUES(14,'AerPower','REAL32',1);"
			"CREATE TABLE IF NOT EXISTS '.data_section' (item INTEGER PRIMARY KEY, begin_number INTEGER, end_number INTEGER);"
			"DELETE FROM sqlite_sequence;"
			"INSERT INTO sqlite_sequence VALUES('data',0);";


		LD("sqlite SQL: " + sql);
        char *err_msg = 0;
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK ) {
        	LD("SQL error: " + err_msg);
            sqlite3_free(err_msg);
        } else {
        	DataSet history {ctx.m_db.getHistory()};
        	int cnt = 1;
        	for (const auto &r : history) {
        		std::u16string ws;
        		std::string bytes;

        		std::string sql1 =
        				std::string("INSERT INTO data VALUES(") +
						std::to_string(cnt++) + ", " +
						r.at(to_base(History::TimeStamp)) + ", ";

        		ws = Utils::utf8_to_u16string(r.at(to_base(History::LastName)));
        		bytes = Utils::bytes2Hex(reinterpret_cast<const unsigned char*>(ws.data()), std::min(40u, ws.size() * 2), false);
        		sql1 += "X'" + bytes + "', ";

        		ws = Utils::utf8_to_u16string(r.at(to_base(History::FirstName)));
        		bytes = Utils::bytes2Hex(reinterpret_cast<const unsigned char*>(ws.data()), std::min(40u, ws.size() * 2), false);
        		sql1 += "X'" + bytes + "', ";

        		ws = Utils::utf8_to_u16string(r.at(to_base(History::MiddleName)));
        		bytes = Utils::bytes2Hex(reinterpret_cast<const unsigned char*>(ws.data()), std::min(40u, ws.size() * 2), false);
        		sql1 += "X'" + bytes + "', ";

        		ws = Utils::utf8_to_u16string(r.at(to_base(History::Profile)));
        		bytes = Utils::bytes2Hex(reinterpret_cast<const unsigned char*>(ws.data()), std::min(40u, ws.size() * 2), false);
        		sql1 += "X'" + bytes + "', ";

        		int duration = std::stoi(r.at(to_base(History::Duration)));
        		sql1 += std::to_string(duration / 3600) + ", ";
        		sql1 += std::to_string(duration / 60 % 60) + ", ";
        		sql1 += std::to_string(duration % 60) + ", ";

        		sql1 += r.at(to_base(History::Distance)) + ", ";
        		sql1 += r.at(to_base(History::Speed)) + ", ";
        		sql1 += r.at(to_base(History::Acceleration)) + ", ";
        		sql1 += r.at(to_base(History::Angle)) + ", ";
        		sql1 += (r.at(to_base(History::HRate)).empty() ? "NULL" : r.at(to_base(History::HRate))) + ", ";
        		sql1 += r.at(to_base(History::Energy)) + ", ";
        		sql1 += r.at(to_base(History::PowerMech)) + ", ";
        		sql1 += r.at(to_base(History::PowerAero));

        		sql1 += ");";

        		LD("sqlite SQL: " + sql1);

				rc = sqlite3_exec(db, sql1.c_str(), 0, 0, &err_msg);

				if (rc != SQLITE_OK ) {
					LD("sqlite SQL error: " + err_msg);
					sqlite3_free(err_msg);
					break;
				}
        	}
        }
    }

	sqlite3_close(db);

}

void DbConnector::importFromEmi(std::string file) {

	DataSet patients {ctx.m_db.getPatients("Номер", true)};
	for (const auto &r : patients) {
		ctx.m_db.deletePatient(stoi(r.at(to_base(DbConnector::PatientList::Id))));
	}

	DataSet profiles {ctx.m_db.getProtocols("USER_PROF")};

	std::ifstream fin (file, std::ios::binary);
	char buff[288];
	DataSet rec;

	while (fin.read(buff, 288)) {
		int ind = 0;
		if (fin.eof()) {
			break;
		}
		uint16_t patient_number;
		char16_t buf16[144];
	    memcpy(&buf16, buff, 288);
	    memcpy(&patient_number, buff, sizeof(uint16_t));
	    ind += 2;
	    std::string last_name {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 40 / 2)).data() };
		ind += 40;
	    std::string first_name {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 40 / 2)).data()};
		ind += 40;
	    std::string middle_name {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 40 / 2)).data()};
		ind += 40;
	    std::string sex {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 2 / 2)).data()};
	    ind += 2;
		uint16_t age;
	    memcpy(&age, buff + ind, sizeof(uint16_t));
	    ind += 2;
		uint16_t weight;
	    memcpy(&weight, buff + ind, sizeof(uint16_t));
	    ind += 2;
	    std::string diagnosis {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 80 / 2)).data()};
		ind += 80;
	    std::string doctor {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 40 / 2)).data()};
		ind += 40;
	    std::string profile {Utils::u16string_to_utf8(std::u16string (buf16 + ind / 2, 40 / 2)).data()};

	    std::string profile_id {"-1"};
	    for (const auto &p : profiles) {
	    	if (p.at(to_base(DbConnector::ProtocolList::Name)) == profile) {
	    		profile_id = p.at(to_base(DbConnector::ProtocolList::Id));
	    		break;
	    	}
	    }

	    DataSet pat = ctx.m_db.getPatient(-1);
	    pat.at(0).at(to_base(PatientList::Number)) = std::to_string(patient_number);
	    pat.at(0).at(to_base(PatientList::Age)) = std::to_string(age);
	    pat.at(0).at(to_base(PatientList::Weight)) = std::to_string(weight);
	    pat.at(0).at(to_base(PatientList::Sex)) = sex;
	    pat.at(0).at(to_base(PatientList::Diagnosis)) = diagnosis;
	    pat.at(0).at(to_base(PatientList::Doctor)) = doctor;
	    pat.at(0).at(to_base(PatientList::FirstName)) = first_name;
	    pat.at(0).at(to_base(PatientList::LastName)) = last_name;
	    pat.at(0).at(to_base(PatientList::MiddleName)) = middle_name;
	    pat.at(0).at(to_base(PatientList::ProfileId)) = profile_id;

//		LD("pos:" + std::to_string(fin.tellg()) + "   pnum: " + std::to_string(patient_number) + "  last:{" + last_name + "}  first:" + first_name + "   middle:" + middle_name);
//		LD("sex:" + sex + "  age:" + std::to_string(age) + "   weight: " + std::to_string(weight) + "  diag:" + diagnosis + "  doc:" + doctor + "   profile:" + profile);

		ctx.m_db.insertPatient(pat);
	}
	fin.close();
}

void DbConnector::upgrade() {
	int current_db_version = stoi(getParam("db_version", "0"));

	if (current_db_version < 1) {
		command("ALTER TABLE `REATERRA`.`Параметры` CHANGE COLUMN `Значение` `Значение` VARCHAR(255) NOT NULL ;");
		setParam("db_version", "1");
	}
}
