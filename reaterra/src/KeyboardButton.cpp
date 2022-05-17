#include "KeyboardButton.h"


bool KeyboardButton::into(int xt, int yt) {
	bool ret = false;

	if (!name.empty() && xt > x && xt < x + width && yt > y && yt < y + height) {
		ret = true;
	}
	return ret;
}
