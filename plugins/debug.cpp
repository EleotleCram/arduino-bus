#include <Arduino.h>

#include "debug.h"

void init_plugin_debug() {
#if DEBUG_LEVEL > OFF
	Serial.begin(115200L);
#endif
}
