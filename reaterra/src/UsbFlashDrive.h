#ifndef USBFLASHDRIVE_H_
#define USBFLASHDRIVE_H_

#include <string>

/// Управление USB-диском.
class UsbFlashDrive {

	friend class DeviceMonitor;

public:


	/// Деструктор.
	virtual ~UsbFlashDrive();

	/*!
	 * @brief Проверка работоспособности.
	 *
	 * @return true: устройство работоспособно.
	 */
	bool isValid() const;

	/*!
	 * @brief Копирование файла.
	 *
	 * @param src откуда.
	 * @param dst куда.
	 */
	void fileCopy(std::string src, std::string dst);

	/*!
	 * @brief Запрос свободного места.
	 *
	 * @return размер свободного места.
	 */
	unsigned long int getFreeSpace();

	/*!
	 * @brief Проверка существования файла.
	 *
	 * @param name имя файла.
	 * @return true: файл существует.
	 */
	bool isFileExisted(std::string name);

	/// Монтирование устройства.
	void driveMount();

	/// Размонтирование устройства.
	void driveUmount();


private:

	bool m_valid;
	std::string m_dev_node;
	std::string m_path;

	UsbFlashDrive(std::string dev_node);

};

#endif /* USBFLASHDRIVE_H_ */
