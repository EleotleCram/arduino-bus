#pragma once

#include "../ABus.h"

typedef enum {
	MSG_RGBLED_ON = ABUS_MSG_BASE(RGBLED),
	MSG_RGBLED_OFF
} rgbled_message_t;

