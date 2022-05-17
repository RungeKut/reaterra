#ifndef GRAPHICENGINE_H_
#define GRAPHICENGINE_H_

#include <vector>

#include <cairo/cairo.h>
#include <linux/fb.h>

#include "KeyboardButton.h"
#include "PictureManager.h"

/*!
 * @brief Перевод раздельных компонентов цвета в композитный.
 *
 * @param r красная компонента (0-255).
 * @param g зеленая компонента (0-255).
 * @param b синяя компонента (0-255).
 * @return композитное значение.
 */
constexpr uint32_t DEC2RGB(uint32_t r, uint32_t g, uint32_t b) {
	return (r << 16) + (g << 8) + b;
}

/*!
 * @brief Выделение красной компоненты из композита.
 *
 * @param rgb композитное значение.
 * @return красная компонента (0-255).
 */
constexpr float RGB2Rd(uint32_t rgb) {
	return (rgb >> 16) & 0xFF;
}

/*!
 * @brief Выделение красной нормированной компоненты из композита.
 *
 * @param rgb композитное значение.
 * @return красная нормированная компонента (0..1).
 */
constexpr float RGB2R(uint32_t rgb) {
	return RGB2Rd(rgb) / 255.;
}

/*!
 * @brief Выделение зеленой компоненты из композита.
 *
 * @param rgb композитное значение.
 * @return зеленая компонента (0-255).
 */
constexpr float RGB2Gd(uint32_t rgb) {
	return (rgb >> 8) & 0xFF;
}

/*!
 * @brief Выделение зеленой нормированной компоненты из композита.
 *
 * @param rgb композитное значение.
 * @return зеленая нормированная компонента (0..1).
 */
constexpr float RGB2G(uint32_t rgb) {
	return RGB2Gd(rgb) / 255.;
}

/*!
 * @brief Выделение синей компоненты из композита.
 *
 * @param rgb композитное значение.
 * @return синяя компонента (0-255).
 */
constexpr float RGB2Bd(uint32_t rgb) {
	return rgb & 0xFF;
}

/*!
 * @brief Выделение синей нормированной компоненты из композита.
 *
 * @param rgb композитное значение.
 * @returnсиняя нормированная компонента (0..1).
 */
constexpr float RGB2B(uint32_t rgb) {
	return RGB2Bd(rgb) / 255.;
}

/// Выравнивание текста.
enum class Align {
	Left,		///< Влево.
	Right,		///< Вправо.
	Center,		///< По центру.
	Top,		///< По верхнему краю.
	Bottom		///< По нижнему краю.
};

/// Стиль шрифта.
enum class FontStyle {
	Normal,		///< Нормальный.
	Bold		///< Жирный.
};

/// Размер контура.
struct SizeGraph {
	int width;	///< Ширина.
	int height;	///< Высота.
};

/// Графический движок.
class GraphicEngine {

public:

	/// Конструктор.
	GraphicEngine();

	/// Деструктор.
	virtual ~GraphicEngine();

	/// Функция обратного вызова для библиотеки Cairo.
	void cairo_linuxfb_surface_destroy();

	/// Функция обратного вызова для библиотеки Cairo.
	void cairo_png_writer(const unsigned char *data, unsigned int length);

	/*!
	 * @brief Отобразить картинку.
	 *
	 * @param surface ссылка на картинку.
	 * @param x координата х верхнего левого угла.
	 * @param y координата y верхнего левого угла.
	 */
	void drawSurface(cairo_surface_t *surface, int x = 0, int y = 0);

	/*!
	 * @brief Отображение одной строки текста.
	 *
	 * @param text текст для отображения.
	 * @param x	координата x.
	 * @param y координата y.
	 * @param size размер шрифта.
	 * @param r красная компонента цвета.
	 * @param g зеленая компонента цвета.
	 * @param b синяя компонента цвета.
	 * @param align выравнивание.
	 * @param style стиль.
	 * @param angle угол поворота.
	 */
	void drawText(std::string text, int x, int y, int size, float r, float g, float b, Align align = Align::Center,
			FontStyle style = FontStyle::Normal, float angle = 0);

	/// Отображение подготовленного изображения.
	void show();

	/*!
	 * @brief Отображение прямоугольника.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param width ширина.
	 * @param height высота.
	 * @param fill_color цвет заполнения.
	 * @param line_width толщина линии.
	 * @param line_color цвет линии.
	 * @param xt координата x касания.
	 * @param yt координата y касания.
	 * @param high_color цвет заполнения подсветки.
	 */
	void drawRectangle(int x, int y, int width, int height, uint32_t fill_color = DEC2RGB(100, 100, 100),
			int line_width = 0, uint32_t line_color = DEC2RGB(200, 200, 200), int xt = 0, int yt = 0, uint32_t high_color = DEC2RGB(200, 200, 200));

	/*!
	 * @brief Отображение кнопки клавиатуры.
	 *
	 * @param button ссылка на кнопку.
	 */
	void drawKbdButton(const KeyboardButton &button);

	/*!
	 * @brief Отображение нескольких строк текста.
	 *
	 * @param text текст для отображения.
	 * @param x координата x.
	 * @param y координата y.
	 * @param width ширина текста.
	 * @param lines количество отображаемых строк.
	 * @param dy промежуток между строками.
	 * @param font_size размер шрифта.
	 * @param h_align выравнивание по горизонтали.
	 * @param v_align выравнивание по вертикали.
	 * @param style стиль.
	 * @param color цвет текста.
	 * @param angle угол поворота.
	 */
	void drawMultiText(std::string text, int x, int y, int width, int lines, int dy, int font_size, Align h_align = Align::Center, Align v_align = Align::Center,
			FontStyle style = FontStyle::Normal, uint32_t color = DEC2RGB(200, 200, 200), float angle = 0);

	/*!
	 * @brief Оценка размера выводимого текста.
	 *
	 * @param text текст для отображения.
	 * @param font_size размер шрифта.
	 * @param style стиль.
	 * @return размер текста.
	 */
	SizeGraph getTextSize(std::string text, int font_size, FontStyle style);

	/*!
	 * @brief Отображение линии.
	 *
	 * @param x1 координата x начала.
	 * @param y1 координата y начала.
	 * @param x2 координата x конца.
	 * @param y2 координата y конца.
	 * @param line_width толщина линии.
	 * @param line_color цвет линии.
	 */
	void drawLine(int x1, int y1, int x2, int y2, int line_width = 1, uint32_t line_color = DEC2RGB(200, 200, 200));

	/*!
	 * @brief Отображение линии с градиентом.
	 *
	 * @param x1 координата x начала.
	 * @param y1 координата y начала.
	 * @param x2 координата x конца.
	 * @param y2 координата y конца.
	 * @param line_width толщина линии.
	 * @param color список цветов градиента.
	 */
	void drawLineGradient(int x1, int y1, int x2, int y2, int line_width, std::vector<std::tuple<float, uint32_t>> color);

	/*!
	 * @brief Получение цвета градиента.
	 *
	 * @param beg первый цвет.
	 * @param end второй цвет.
	 * @param index точка градиента (0..1).
	 * @return
	 */
	uint32_t getGradientColor(uint32_t beg, uint32_t end, float index);

private:
	typedef struct _cairo_linuxfb_device {
		int fb_fd;
		unsigned char *fb_data;
		long fb_screensize;
		struct fb_var_screeninfo fb_vinfo;
		struct fb_fix_screeninfo fb_finfo;
	} cairo_linuxfb_device_t;

	static const int SCREEN_WIDE = 1280;
	static const int SCREEN_HEIGHT = 800;
	static constexpr int PIXELS_IN_SCREEN = SCREEN_WIDE * SCREEN_HEIGHT;

	bool m_good;
	cairo_linuxfb_device_t device;
	cairo_surface_t *fbsurface;
	cairo_t *fbcr;
	int bufid;
	int fd;
	std::vector<unsigned char> buff;
	uint32_t fbuff[8192000 / 4] __attribute__((aligned(4)));
	cairo_surface_t *memsurf;
	cairo_t *memcr;
	cairo_t *cr;
	bool m_remote_screen;


	int flip_buffer(const bool vsync, const bool to_first_page = false);
	cairo_surface_t *cairo_linuxfb_surface_create(const char *fb_name);
	std::vector<std::string> splitString(std::string str, int width, bool use_font = false, int font_size = 24, FontStyle font_style = FontStyle::Normal);
	void writeScreen(int x, int y, int w, int h);
	void showRemoteScreen();


};

#endif /* GRAPHICENGINE_H_ */
