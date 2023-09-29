
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <string.h>

#include "lownet.h"
#include "serial_io.h"
#include "utility.h"

#include "app_chat.h"

void chat_receive(const lownet_frame_t* frame) {
	for (size_t i = 0; i < frame->length; i++) { // Check if all characters are valid
        if (!util_printable(frame->payload[i])) {
            printf("Invalid character in message received from Node 0x%02X: %c\n", frame->source, frame->payload[i]);
            return; // Do not print the message.
        }
    }

	if (frame->destination == lownet_get_device_id()) {
		// This is a tell message, just for us!
		printf("Tell message received from Node %u: %s\n", frame->source, frame->payload);
	} else {
		// This is a broadcast shout message.
		printf("Shout message received from Node %u: %s\n", frame->source, frame->payload);
	}
}

void chat_shout(const char* message) {
	for (size_t i = 0; i < strlen(message); i++) { // Check if all characters are valid
        if (!util_printable(message[i])) {
            printf("Invalid character in message: %c\n", message[i]);
            return; // Do not print the message.
        }
    }

	lownet_frame_t frame;
	frame.source = lownet_get_device_id();
	frame.destination = 0xFF;
	frame.protocol = LOWNET_PROTOCOL_CHAT;
	frame.length = strlen(message);
	memcpy(frame.payload, &message, frame.length);

	lownet_send(&frame);
	printf("Message shouted");
}

void chat_tell(const char* message, uint8_t destination) {
	for (size_t i = 0; i < strlen(message); i++) { // Check if all characters are valid
        if (!util_printable(message[i])) {
            printf("Invalid character in message: %c\n", message[i]);
            return; // Do not print the message.
        }
    }

	lownet_frame_t frame;
	frame.source = lownet_get_device_id();
	frame.destination = destination;
	frame.protocol = LOWNET_PROTOCOL_CHAT;
	frame.length = strlen(message);
	memcpy(frame.payload, &message, frame.length);

	lownet_send(&frame);
	printf("Message sent to %u:\n.", destination);
}