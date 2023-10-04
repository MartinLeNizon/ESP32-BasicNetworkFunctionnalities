
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <string.h>

#include "lownet.h"
#include "serial_io.h"
#include "utility.h"

#include "fake.h"

void fake(const uint8_t destination) {
	char* message = "Hello, this is the master node";
	uint8_t source = 0xE0;

	lownet_frame_t frame;
	frame.source = source;
	frame.destination = destination;
	frame.protocol = LOWNET_PROTOCOL_CHAT;
	frame.length = strlen(message);
	memcpy(frame.payload, message, frame.length);

	lownet_send(&frame);
	printf("Message sent to Node 0x%02X faking 0x%02X: %s\n", destination, source,message);
}