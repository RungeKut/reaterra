#ifndef SRC_GPIODRIVER_H_
#define SRC_GPIODRIVER_H_

/// Управление портами ввода-вывода.
namespace GpioDriver {

/// Направление.
enum class PinDirection {
	In, 	///< Ввод.
	Out		///< Вывод.
};


/// Уровень.
enum class PinLevel {
	Low,	///< Низкий.
	High	///< Высокий.
};

/*!
 * @brief Установка направления.
 *
 * @param pin номер пина.
 * @param dir направление.
 */
void setPinDirection(int pin, PinDirection dir);

/*!
 * @brief Установка уровня.
 *
 * @param pin номер пина.
 * @param level уровень.
 */
void setPinLevel(int pin, PinLevel level);

/*!
 * @brief Получить уровень.
 *
 * @param pin номер пина.
 * @return уровень.
 */
PinLevel getPinLevel(int pin);

}


#endif /* SRC_GPIODRIVER_H_ */
