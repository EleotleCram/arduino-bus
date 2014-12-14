#pragma once

#include <cstdint>
#include <functional>

#define ABUS_MSG_BASE(X) (MSG_BASE_##X)

namespace abus {

typedef std::function<void(const uint16_t, const void*)> msgfunc;

#define MAX_NUM_SUBSCRIBERS 12
class ABus {
private:
	int numsubscribers = 0;
	abus::msgfunc subscribers[MAX_NUM_SUBSCRIBERS];

public:
	void subscribe(abus::msgfunc f) {
		if(numsubscribers < MAX_NUM_SUBSCRIBERS) {
			subscribers[numsubscribers++] = f;
		}
	}

	void send(uint16_t msgId) {
		send(msgId, 0);
	}
	void send(uint16_t msgId, void* pArg) {
		for(uint8_t i = 0; i < numsubscribers; i++) {
			subscribers[i](msgId, pArg);
		}
	}
};
#undef MAX_NUM_SUBSCRIBERS

} // namespace abus

extern abus::ABus ABus;
