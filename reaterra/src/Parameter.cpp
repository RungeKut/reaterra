#include "Parameter.h"
#include "Logger.h"


Parameter::Parameter(std::map<std::string, std::string> default_values)
:	m_parameters {default_values}
{
	LD("Ctor");
}

Parameter::~Parameter() {
	LD("Dtor");
}


void Parameter::setParamString(std::string name, std::string value) {
	m_parameters[name] = value;
	m_changed = true;
}

std::string Parameter::getParamString(std::string name) {
	std::string ret;
	try {
		ret = m_parameters.at(name);
	} catch (const std::out_of_range& e) {
		LE("Error! Parameter <" + name + "> is not found");
		throw std::runtime_error("Error! Parameter <" + name + "> is not found");
	}

	return ret;
}

float Parameter::getParamFloat(std::string name) {
	std::string str_value = getParamString(name);
	return std::stof(str_value);
}

int Parameter::getParamInt(std::string name) {
	std::string str_value = getParamString(name);
	return std::stoi(str_value);
}


//void Parameter::update(std::map<std::string, std::string> data) {
//	for (const auto &param : m_parameters) {
//		if (!m_parameters.find(param.first)) {
//			m_parameters[param.first] = param.second;
//		}
//	}
//}
