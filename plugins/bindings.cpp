#include "debug.h"
#include "rc.h"
#include "rgbled.h"

#include "../ABus.h"

#include <cstdint>

static bool status = true;
void init_plugin_bindings() {
	ABus.subscribe([&] (const uint16_t msgId, const void* pArg) {
		// handle message
		switch(msgId) {
			case MSG_RC_CODE:
				LOG_INFO("MSG_RC_CODE");
				uint32_t code = *(uint32_t*)pArg;
				if(code == 0xFF02FD) {
					status = !status;
					if(status) {
						ABus.send(MSG_RGBLED_ON);
					} else {
						ABus.send(MSG_RGBLED_OFF);
					}
				}
				break;
		}
	});
}
