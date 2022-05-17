#ifndef LOCALE_H_
#define LOCALE_H_

#include <map>
#include <string>

/*!
 * @brief Глобальная функция локализации.
 *
 * @param str ключевой текст.
 * @param extra вторичный ключ.
 * @return локализованная строка.
 */
std::string M(std::string str, std::string extra = "");

/// Локализатор.
class Locale {

public:

	/// Получение объекта.
	static Locale& instance();

	/*!
	 * @brief Выполнение локализации.
	 *
	 * @param str ключевой текст.
	 * @param extra extra вторичный ключ.
	 * @return локализованная строка.
	 */
	std::string tr(std::string str, std::string extra = "");

	/*!
	 * @brief Инициализация.
	 *
	 * @param locale язык локализации.
	 */
	void init(std::string locale);

	/*!
	 * @brief Получение языка локализации.
	 *
	 * @return язык локализации.
	 */
	std::string getLocale();

private:
	Locale();
	virtual ~Locale();
	std::map<std::string, std::string> m_dic;
	bool m_save;
	std::string m_locale;

};

#endif /* LOCALE_H_ */
