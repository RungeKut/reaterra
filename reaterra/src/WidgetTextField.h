#ifndef WIDGETTEXTFIELD_H_
#define WIDGETTEXTFIELD_H_

#include "Widget.h"

#include <functional>

#include "EnumExtension.h"
#include "GraphicEngine.h"


/// Ширина экрана.
const static int SCREEN_WIDTH = 1280;

/// Высота экрана.
const static int SCREEN_HEIGHT = 800;

/// Тип используемой клавиатуры.
enum class KbdType {
	Alpha = 1,			///< Алфавитно-цифровая.
	Digital = 2,		///< Цифровая.
	Date = 4,			///< Ввод даты.
	None = 8,			///< Клавиатура отключена.
	MinusFlag = 0x100,	///< Использование знака МИНУС.
	DotFlag = 0x200		///< Использование десятичной точки.
};
ENABLE_BITMASK_OPERATORS(KbdType)

class WidgetTextField;

/// Тип функции проверки ввода.
typedef std::function<std::string(std::string, WidgetTextField&)> CheckFunc;

/// Тип функции вызываемой при отображении виджета.
typedef std::function<std::string(WidgetTextField&)> UpdateFunc;

/// Структура стиля виджета.
typedef struct {
	std::string text;				///< Текст виджета.
	std::string prompt;				///< Подсказка ввода.
	Align halign;					///< Горизонтальное выравнивание.
	Align valign;					///< Вертикальное выравнивание.
	unsigned lines;					///< Максимальное количество строк текста.
	unsigned dy;					///< Расстояние между строками.
	FontStyle font_style;			///< Стиль шрифта.
	int font_size;					///< Размер шрифта.
	float text_rotate;				///< Угол поворота текста в радианах.
	uint32_t text_color_active;		///< Цвет активного текста.
	uint32_t text_color_passive;	///< Цвет текста.
	uint32_t text_color_disabled;	///< Цвет заблокированного текста.
	uint32_t rect_color_active;		///< Цвет активного поля текста.
	uint32_t rect_color_passive;	///< Цвет поля текста.
	uint32_t rect_color_disabled;	///< Цвет заблокированного поля текста.
	std::string img_file_active;	///< Картинка активного фона.
	std::string img_file_passive;	///< Картинка фона.
	std::string img_file_disabled;	///< Картинка заблокированного фона.
	CheckFunc check_func;			///< Функция, вызываемая при вводе текста.
	UpdateFunc update_func;			///< Функция, вызываемая при отображении виджета.
	KbdType kbd_type;				///< Тип клавиатуры.
	int text_max_size;				///< Максимальная длина текста.
	float max_limit;				///< Верхний предел при вводе числа.
	float min_limit;				///< Нижний предел при вводе числа.
	std::string limit_format;		///< Формат вывода пределов.
	std::string password_char;		///< Символ отображения пароля.
	std::string num_format;			///< Формат отображения чисел.
	int hpad;						///< Отступ от края справа и слева.
} WTFStyle;

/// Стить по умолчанию.
extern WTFStyle default_style;

/// Виджет многострочного текста с клавиатурой.
class WidgetTextField: public Widget {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param x координата x.
	 * @param y координата y.
	 * @param width ширина текста.
	 * @param height высота текста.
	 * @param style стиль виджета.
	 * @param text отображаемый текст.
	 * @param prompt подсказка ввода.
	 * @param checkfunc функция проверки ввода.
	 */
	WidgetTextField(int x, int y, int width, int height, WTFStyle style = default_style, std::string text = "", std::string prompt = "", CheckFunc checkfunc = CheckFunc {});

	/// Деструктор.
	virtual ~WidgetTextField();

	/// Отрисовка изображения.
	void show() override;

	/*!
	 * @brief Установка текста виджета.
	 *
	 * @param text текст виджета.
	 * @param id идентификатор.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setText(const std::string& text, const std::string &id = "");

	/*!
	 * @brief Получение текста виджета.
	 *
	 * @return текст виджета.
	 */
	std::string getText() const;

	/*!
	 * @brief Установка функции обновления.
	 *
	 * @param update_func функция обновления.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setUpdateFunc(UpdateFunc update_func);

	/*!
	 * @brief Установка функции проверки.
	 *
	 * @param check_func функция проверки.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setCheckFunc(CheckFunc check_func);

	/*!
	 * @brief Установка подсказки ввода.
	 *
	 * @param prompt подсказка ввода.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setPrompt(std::string prompt);

	/*!
	 * @brief Установка пределов ввода числа.
	 *
	 * @param min нижний предел.
	 * @param max верхний предел.
	 * @param format формат вывода пределов.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setLimits(float min, float max, std::string format = "");

	/*!
	 * @brief Установка верхнего предела ввода числа.
	 *
	 * @param max верхний предел ввода числа.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setMaxLimit(float max);

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(::Event *event) override;

	/*!
	 * @brief Получение идентификатора столбца.
	 *
	 * @return идентификатор столбца.
	 */
	int getIdCol() const;

	/*!
	 * @brief Установка идентификатора столбца.
	 *
	 * @param idCol идентификатор столбца.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setIdCol(int idCol);

	/*!
	 * @brief Получение идентификатора строки.
	 *
	 * @return идентификатор строки.
	 */
	int getIdRow() const;

	/*!
	 * @brief Установка идентификатора строки.
	 *
	 * @param idRow идентификатор строки.
	 * @return ссылка на виджет.
	 */
	WidgetTextField &setIdRow(int idRow);

	/*!
	 * @brief Получение идентификатора текста.
	 *
	 * @return идентификатор текста.
	 */
	std::string getTextId() const;

	/*!
	 * @brief Установка размера текста.
	 *
	 * @param size размер текста.
	 */
	void setFontSize(unsigned size);

private:

	WTFStyle m_style;
	bool m_trigger;
	std::wstring m_editor_string;
	int m_id_row;
	int m_id_col;
	int m_active_picture_id;
	int m_passive_picture_id;
	int m_disabled_picture_id;
	std::string m_text_id;


};

typedef std::shared_ptr<WidgetTextField> WidgetTextFieldSP;
typedef std::vector<WidgetTextFieldSP> WidgetTextFieldSPVec;

#include "Styles.h"

#endif /* WIDGETTEXTFIELD_H_ */
