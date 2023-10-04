
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

#include "globals.h"

int message_is_printable(const char* message, const uint8_t length) {

	for (size_t i = 0; i < length; i++) { // Check if all characters are valid
        if (!util_printable(message[i])) {
            return 0;
        }
    }

    return 1;
}

void chat_receive(const lownet_frame_t* frame) {
	if (!message_is_printable((char*) frame->payload, frame->length)) {
		if (!mute_mode) {
			printf("Error when receiving a message: non-printable character\n");
		}

		return;
	}

	if (frame->destination == lownet_get_device_id()) {
		// This is a tell message, just for us!
		printf("Tell message received from Node 0x%02X: %.*s\n", frame->source, frame->length, frame->payload);
	} else {
		// This is a broadcast shout message.
		if (!mute_mode) {
			printf("Shout message received from Node 0x%02X: %.*s\n", frame->source, frame->length, frame->payload);
		}
	}
	
}

void chat_shout(const char* message) {
	if (!message_is_printable(message, strlen(message))) {
		printf("Error when shouting the message: non-printable character\n");
		return;
	}

	chat_tell(message, 0xFF);
	printf("Message shouted: %s\n", message);
}

void chat_tell(const char* message, uint8_t destination) {
	if (destination != 0xFF && !message_is_printable(message, strlen(message))) {
		printf("Error when telling the message: non-printable character\n");
		return;
	}

	lownet_frame_t frame;
	frame.source = lownet_get_device_id();
	frame.destination = destination;
	frame.protocol = LOWNET_PROTOCOL_CHAT;
	frame.length = strlen(message);
	memcpy(frame.payload, message, frame.length);

	lownet_send(&frame);
	if (destination != 0xFF) {
		printf("Message sent to Node 0x%02X: %s\n", destination, message);
	}
}