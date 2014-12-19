#include <Arduino.h>

#include "loop.h"
#include "debug.h"
#include <abus/ABus.h>

void init_plugin_loop() {}

extern "C" void loop() {
	ABus.send(MSG_LOOP_TICK);
#if DEBUG_LEVEL >= _DEBUG_
	delay(1000);
#else
	delay(1);
#endif
}
