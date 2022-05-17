#ifndef PICTUREMANAGER_H_
#define PICTUREMANAGER_H_

#include <cairo/cairo.h>
#include <map>
#include <memory>
#include <string>

/// Управление картинками.
class PictureManager {
public:

	/// Конструктор.
	PictureManager();

	/// Деструктор.
	virtual ~PictureManager();

	/*!
	 * @brief Загрузка картинки.
	 *
	 * @param file имя файла.
	 * @return идентификатор картинки, 0 в в случае ошибки.
	 */
	int loadPicture(std::string file);

	/*!
	 * Получение картинки.
	 *
	 * @param id идентификатор.
	 * @return ссылка на картинку.
	 */
	cairo_surface_t *getPicture(int id);


private:

	struct Picture {
		int id;
		std::string file;
		std::shared_ptr<cairo_surface_t> image;
	};

	std::map<int, Picture> m_pic_storage;
	int m_id_counter;

};

#endif /* PICTUREMANAGER_H_ */
