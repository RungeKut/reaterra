
#include "Locale.h"
#include "WidgetTextField.h"



WTFStyle default_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	0, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	30, 				// font_size
	0.,					// text rotate angle
	0x5A5A5A, 			// text_color_active
	0x5A5A5A, 			// text_color_passive
	0x5A5A5A, 			// text_color_disabled
	0xACD5E6, 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::Alpha,		// kbd_type
	255,				// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};


WTFStyle spinner_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	0, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	70, 				// font_size
	0.,					// text rotate angle
	0x5A5A5A, 			// text_color_active
	0x5A5A5A, 			// text_color_passive
	0x5A5A5A, 			// text_color_disabled
	0xACD5E6, 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::Digital | KbdType::DotFlag,	// kbd_type;
	5,					// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};


WTFStyle patient_footer_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Left, 		// halign
	Align::Center, 		// valign
	1, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	29, 				// font_size
	0.,					// text rotate angle
	0x434242, 			// text_color_active
	0x434242, 			// text_color_passive
	0x434242, 			// text_color_disabled
	0xACD5E6, 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::Alpha,		// kbd_type;
	255,					// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	0					// hpad
};


WTFStyle rehab_user_protocol_table_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	2, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	25, 				// font_size
	0.,					// text rotate angle
	0x0, 			// text_color_active
	0x0, 			// text_color_passive
	0x0, 			// text_color_disabled
	0xACD5E6, 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::Alpha,		// kbd_type;
	255,					// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};

WTFStyle back_style {
	M("НАЗАД"), 		// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	0, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	19, 				// font_size
	0.,					// text rotate angle
	to_base(Colors::White), 			// text_color_active
	to_base(Colors::White), 			// text_color_passive
	to_base(Colors::White), 			// text_color_disabled
	to_base(Colors::None), 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::None,		// kbd_type
	255,				// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};

WTFStyle bc_link_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	1, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	22, 				// font_size
	0.,					// text rotate angle
	0xCC9933, 			// text_color_active
	0x87CEE8, 			// text_color_passive
	0x87CEE8, 			// text_color_disabled
	to_base(Colors::None), 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::None,		// kbd_type
	255,				// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};

WTFStyle bc_last_style {
	"", 				// m_text;
	"", 				// m_prompt;
	Align::Center, 		// halign
	Align::Center, 		// valign
	1, 					// lines 0- nolimit
	0, 					// dy  0 - auto = 1.2 * font_size
	FontStyle::Normal, 	// font_style
	24, 				// font_size
	0.,					// text rotate angle
	0xCC9933, 			// text_color_active
	0x87CEE8, 			// text_color_passive
	to_base(Colors::White), 			// text_color_disabled
	to_base(Colors::None), 			// rect_color_active
	to_base(Colors::None), // rect_color_passive
	to_base(Colors::None), // rect_color_disabled
	"", 				// img_file_active - empty not used
	"", 				// img_file_passive - empty not used
	"", 				// img_file_disabled - empty not used
	CheckFunc {}, 		// check_func
	UpdateFunc {}, 		// update_func
	KbdType::None,		// kbd_type
	255,				// text_max_size
	0, 					// max_limit
	0, 					// min_limit
	"",					// limit_format
	"",					// password_char - empty string for nothing
	"",					// num_format
	5					// hpad
};

