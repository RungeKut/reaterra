#ifndef TYPEPARSER_H_
#define TYPEPARSER_H_

#include "Menu.h"

/// Создание объектов по имени класса.
namespace TypeParser {

	std::unique_ptr<Menu> createObject(std:: string type_name);

}



#endif /* TYPEPARSER_H_ */
