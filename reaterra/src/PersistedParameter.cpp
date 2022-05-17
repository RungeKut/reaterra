#include "PersistedParameter.h"

#include <fstream> 		// ifstream ofstream
#include <algorithm> 	// remove_if

#include "Logger.h"

PersistedParameter::PersistedParameter(std::string file_name, std::map<std::string, std::string> default_values)
:	Parameter(default_values),
	m_file_name {file_name}
{
	LD("Ctor " + m_file_name);
	load();
}

PersistedParameter::~PersistedParameter() {
//	if (m_changed) {
		save();
//	}
	LD("Dtor " + m_file_name);
}

void PersistedParameter::load() {
	std::ifstream in(m_file_name);

	if (!in.good()) {
		LI("Parameters file (" + m_file_name + ") not found. Parameters will be default.");
		m_changed = true;
		return;
	}

	for (std::string line; std::getline(in, line); ) {
		if (line.empty()) {
			continue;
		}

		int ind = line.find(DELIMITER);
		std::string key = line.substr(0, ind);
		std::string value = line.substr(ind + 1);
		value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char c){ return std::isspace(c); }), value.end());
		m_parameters[key] = value;
	}
}

void PersistedParameter::save() {
	std::ofstream out(m_file_name);

	LD("Parameters are saving in " + m_file_name);
	if (!out.good()) {
		LE("Error writing to parameters file (" + m_file_name + ")!");
		return;
	}

	for (auto elem = m_parameters.begin(); elem != m_parameters.end(); elem++) {
		out << elem->first << DELIMITER << elem->second << "\n";
	}
}
