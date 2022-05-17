#ifndef SRC_PERSISTEDPARAMETER_H_
#define SRC_PERSISTEDPARAMETER_H_

#include <string>		// erase getline

#include "Parameter.h"

/// Сохраняемые параметры.
class PersistedParameter : public Parameter {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param file_name имя файла для хранения параметров.
	 * @param default_values значения по умолчанию.
	 */
	explicit PersistedParameter(std::string file_name, std::map<std::string, std::string> default_values = {});

	/// Деструктор.
	virtual ~PersistedParameter();

	/// Сохранение параметров.
	void save();

private:
	const static char DELIMITER {' '};
	std::string m_file_name;

	void load();

};

#endif /* SRC_PERSISTEDPARAMETER_H_ */
