#pragma once

#include "../ABus.h"

typedef enum {
	MSG_RC_CODE = ABUS_MSG_BASE(RC),
	MSG_RC_REPEAT
} rc_message_t;

