#ifndef SRC_PARAMETER_H_
#define SRC_PARAMETER_H_

#include <map>
#include <string>

/// Работа с параметрами.
class Parameter {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param default_values значения параметров по умолчанию.
	 */
	explicit Parameter(std::map<std::string, std::string> default_values = {});

	// Деструктор.
	virtual ~Parameter();

	/*!
	 * @brief Установка параметра.
	 *
	 * @param name имя параметра.
	 * @param value значение параметра.
	 */
	void setParamString(std::string name, std::string value);

	/*!
	 * Получение параметра в виде строки.
	 *
	 * @param name имя параметра.
	 * @return значение параметра.
	 */
	std::string getParamString(std::string name);

	/*!
	 * Получение параметра в виде числа с плавающей точкой.
	 *
	 * @param name имя параметра.
	 * @return значение параметра.
	 */
	float getParamFloat(std::string name);

	/*!
	 * Получение параметра в виде целого числа.
	 *
	 * @param name имя параметра.
	 * @return значение параметра.
	 */
	int getParamInt(std::string name);


protected:

	std::map<std::string, std::string> m_parameters;
	bool m_changed {false};

//private:
//
//	void update(std::map<std::string, std::string> data);

};

#endif /* SRC_PARAMETER_H_ */
