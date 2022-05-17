#ifndef REHABTESTPROTOCOLMENU_H_
#define REHABTESTPROTOCOLMENU_H_

#include "Context.h"
#include "Menu.h"
#include "WidgetImageButton.h"
#include "WidgetRectangle.h"
#include "WidgetText.h"

/// Меню "тестирование по протоколу ".
class RehabTestProtocolMenu: public Menu {

public:

	/*!
	 * @brief Конструктор.
	 */
	RehabTestProtocolMenu();

	/// Деструктор.
	virtual ~RehabTestProtocolMenu();

	/*!
	 * @brief Функция обработки событий.
	 *
	 * @param event событие.
	 * @return ссылка на новое меню.
	 */
	Menu *processEvent(std::unique_ptr<Event> event) override;

private:

	static const int TABLE_ROWS = 15;
	static const int TABLE_COLS = 4;

	int m_time;
	std::shared_ptr<WidgetImageButton> m_start;
	std::shared_ptr<WidgetImageButton> m_stop;
	std::shared_ptr<WidgetImageButton> m_modif;
	Timer m_timer;
	Timer m_refresh_timer;
	std::vector<std::shared_ptr<WidgetRectangle>> m_high_light;
	int m_rows_start;
	int m_row_step;
	unsigned m_current_interval;
	int m_interval_time;
	DataSet m_intervals_data;
	DataSet m_modifs_list;
	std::vector<std::vector<std::shared_ptr<WidgetText>>> m_table;
	DataSet m_params;
	std::shared_ptr<WidgetText> m_duration_unit;
	std::shared_ptr<WidgetText> m_speed_unit;
	std::shared_ptr<WidgetText> m_angle_unit;
	std::shared_ptr<WidgetText> m_msg_line_1;
	std::shared_ptr<WidgetText> m_msg_line_2;
	std::shared_ptr<WidgetText> m_msg_line_3;
	std::shared_ptr<WidgetText> m_modif_name;
	bool m_not_count;
	bool m_running;
	std::string m_proto_name;
	TreadmillDriver::RunState m_cur_state;

	void turnOffHighLight();
	void highLightRow(int row);
	void stopAction();
	void updateData(int id);

};

#endif /* REHABTESTPROTOCOLMENU_H_ */
