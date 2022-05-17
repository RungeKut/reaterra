#include "Locale.h"

#include <iostream> 		// cout

#include "Context.h"

std::string M(std::string str, std::string extra) {
	return Locale::instance().tr(str, extra);
}


Locale::Locale()
: m_locale {"RU"}
{
	m_save = true;
}

Locale& Locale::instance() {
	static Locale instance;
	return instance;
}

std::string Locale::tr(std::string str, std::string extra) {
	std::string key {extra.empty() ? str : extra};
	auto it = m_dic.find(key);
	if (it != m_dic.end()) {
		if ((*it).second.empty()) {
			return str;
		} else if ((*it).second == "<!~EmPtY!~>") {
			return "";
		} else {
			return (*it).second;
		}
	} else {
		m_dic[str] = "";
		return str;
	}
}

void Locale::init(std::string locale) {
	m_locale = locale;
	m_dic.clear();
//	std::cout << "Locale ctor: lang " << m_locale << std::endl ;

	std::string locale_file = "reaterra_locale_" + m_locale + ".txt";
	std::ifstream in(locale_file);

	if (in.good()) {
		for (std::string line; std::getline(in, line); ) {
			if (line.empty()) {
				continue;
			}

			int ind = line.find("|");
			std::string key = line.substr(1, ind - 2);
			std::string value = line.substr(ind + 2, line.size() - ind - 3);
			m_dic[key] = value;
		}
	}
}

std::string Locale::getLocale() {
	return m_locale;
}

Locale::~Locale() {
	if (m_save) {
		std::string locale_file = "reaterra_locale_" + m_locale + ".txt";
		std::ofstream out(locale_file);

		if (!out.good()) {
			LE("Error writing to locale file!");
			return;
		}

		for (auto elem = m_dic.begin(); elem != m_dic.end(); elem++) {
			out << "\"" << elem->first << "\"|\"" << elem->second << "\"\n";
		}
	}
}

