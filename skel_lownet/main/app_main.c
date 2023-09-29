// #ifndef DEBUG
// #define DEBUG
// #endif

/* -----------------------
Issue with time
Check print_time()
Check ping()
Check ping_receive()
Check chat_tell()
Check chat_shout()
Done - Check chat_receive()
>>Maybe Implement time_receive
>>Check util_printable(char)
Then app_main.c done

Check input errors handles in the first assignment
------------------------*/

// CSTDLIB includes.
#include <stdio.h>
#include <string.h>

// LowNet includes.
#include "lownet.h"

#include "serial_io.h"
#include "utility.h"

#include "app_chat.c"
#include "app_ping.c"

const char* ERROR_OVERRUN = "ERROR // INPUT OVERRUN";
const char* ERROR_UNKNOWN = "ERROR // PROCESSING FAILURE";

const char* ERROR_COMMAND = "Command error";
const char* ERROR_ARGUMENT = "Argument error";

void app_frame_dispatch(const lownet_frame_t* frame) {
	switch (frame->protocol) {
		case LOWNET_PROTOCOL_CHAT:
			chat_receive(frame);
			break;

		case LOWNET_PROTOCOL_PING:
			ping_receive(frame);
			break;
	}
}

void print_time(lownet_time_t time) {
	if (time.seconds == 0 && time.parts == 0) {
        printf("Network time is not available.\n");
	} else {
		// Calculate the total milliseconds (with 10 ms accuracy) since the course started.
		uint8_t centiseconds = (((uint64_t)(time.parts * 1000) / 256 + 5)/10);	// 10 ms accuracy
		if (centiseconds <= 10) { // Handles the case, 16 ms -> 2 cs -> .02 instead of .2 (wrong) 
			printf("%lu.0%u sec since the course started .\n", time.seconds, centiseconds);
		} else {
			printf("%lu.0%u sec since the course started .\n", time.seconds, centiseconds);
		}
	}
}

void app_main(void)
{
	char msg_in[MSG_BUFFER_LENGTH];
	char msg_out[MSG_BUFFER_LENGTH];
	
	// Initialize the serial services.
	init_serial_service();

	// Initialize the LowNet services.
	lownet_init(app_frame_dispatch);

	printf("Own node: %u\n", lownet_get_device_id());

	while (true) {
		memset(msg_in, 0, MSG_BUFFER_LENGTH);
		memset(msg_out, 0, MSG_BUFFER_LENGTH);

		if (!serial_read_line(msg_in)) {
			if (msg_in[0] == '/') {
				char* cmd = strtok(msg_in, " "); // Get the ping or date command, "/" included
				char* arg = strtok(NULL, "\n"); // Get the rest of the line as an argument
				#ifdef DEBUG
					printf("cmd: %s; arg: %s\n", cmd, arg);
                #endif
                if (cmd) {
                    if (strcmp(cmd, "/ping") == 0) {
                    	if (!arg) {
		                	printf("%s\n", ERROR_ARGUMENT); // Unknown argument
		                	continue;
		                }
                    	if (strcmp(arg,"all") == 0) arg = "0xFF"; // ping all connected devices if no arg
                        ping((uint8_t)strtol(arg, NULL, 16));
                    } else if (strcmp(cmd, "/date") == 0) {
                        print_time(lownet_get_time());
                    } else {
                        printf("%s\n", ERROR_COMMAND); // Unknown command
                    }
                } else if (!cmd) {
                	printf("%s\n", ERROR_COMMAND); // Unknown command
                }
			} else if (msg_in[0] == '@') {
				char* dest_node = strtok(msg_in, " "); // Get the dest node, with the @
				dest_node = dest_node + 1; // "+1" to remove the @
				uint8_t destination = (uint8_t)strtol(dest_node, NULL, 16); // Convert hex string to uint8_t
                char* msg = strtok(NULL, "\n"); // Get the rest of the line as a message
                #ifdef DEBUG
                	printf("dest_node: 0x%02X; msg: %s\n", destination, msg);
                #endif
                if (destination == 0xFF) chat_shout(msg);
				else chat_tell(msg, destination);
			} else {
				chat_shout(msg_in + 1); // Remove "@" from the string
			}
		}
	}
}
