#include <Arduino.h>

#include "debug.h"
#include "rc.h"

#include "../ABus.h"

#include <IRLib.h>

int RECV_PIN = 14;

IRrecv My_Receiver(RECV_PIN);

IRdecode My_Decoder;
unsigned int Buffer[RAWBUF];

void init_plugin_rc() {
	My_Receiver.enableIRIn(); // Start the receiver
	My_Decoder.UseExtnBuf(Buffer);

	ABus.subscribe([&] (const uint16_t msgId, const void* pArg) {
		if (My_Receiver.GetResults(&My_Decoder)) {
			//Restart the receiver so it can be capturing another code
			//while we are working on decoding this one.
			My_Receiver.resume();
			My_Decoder.decode();
			if (My_Decoder.decode_type != UNKNOWN) {
				if (My_Decoder.value == 0xFF02FD) {
					LOG_INFO("On/Off");
					ABus.send(MSG_RC_CODE, &My_Decoder.value);
				} else {
					My_Decoder.DumpResults();
				}
			}
		}
	});
}
