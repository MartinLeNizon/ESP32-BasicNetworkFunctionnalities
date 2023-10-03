#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app_ping.h"

#include "lownet.h"
#include "serial_io.h"

const uint16_t MAX_PING_MS = 60000; // 1 minute

typedef struct __attribute__((__packed__))
{
	lownet_time_t 	timestamp_out;
	lownet_time_t	timestamp_back;
	uint8_t 		origin;
} ping_packet_t;


void ping(uint8_t node) {
	ping_packet_t ping_packet;

	// Get the current network time for the timestamp_out field
	lownet_time_t time = lownet_get_time();
	if (time.seconds == 0 && time.parts == 0) {
		printf("Network time is not available. Ping failed.\n");
		return;
	} else {
		ping_packet.timestamp_out = time;
	}

	// ping_packet.timestamp_back remains not initialized; unused for the outward way

	ping_packet.origin = lownet_get_device_id();

	// Create a LowNet frame to send the ping packet
	lownet_frame_t ping_frame;
	ping_frame.source = lownet_get_device_id();
    ping_frame.destination = node;
    ping_frame.protocol = LOWNET_PROTOCOL_PING;
    ping_frame.length = sizeof(ping_packet);
    memcpy(ping_frame.payload, &ping_packet, sizeof(ping_packet)); // Copy the ping packet to the frame payload.

    lownet_send(&ping_frame);

    if (node == 0xFF) {
    	printf("Pinging everyone\n");
    } else {
		printf("Pinging 0x%02X:\n", node);
    }
}


void ping_receive(const lownet_frame_t* frame) {
	if (frame->length < sizeof(ping_packet_t)) {
		return; // Malformed frame.  Discard.
	}

	// Extract the ping packet from the frame payload
	ping_packet_t received_ping;
	memcpy(&received_ping, frame->payload, sizeof(ping_packet_t));

	if (received_ping.origin == lownet_get_device_id()) { // If it's the pong

		uint32_t round_trip_ms = (lownet_get_time().seconds - received_ping.timestamp_out.seconds) * 1000 + ( (uint32_t) (lownet_get_time().parts - received_ping.timestamp_out.parts) * 1000 / 256);

		// Check if the round-trip time exceeds the maximum allowed time
		if (round_trip_ms > MAX_PING_MS) {
			printf("Round-trip time exceeds maximum allowed time.\n");
		} else {
			printf("Ping reply from 0x%02X; RTT=%lums\n", frame->source, round_trip_ms);
		}

	} else { // If it's the ping

		ping_packet_t ping_packet = received_ping;

		uint8_t dest = received_ping.origin;

		// Get the current network time for the timestamp_out field
		lownet_time_t time = lownet_get_time();
		if (time.seconds == 0 && time.parts == 0) {
			printf("Network time is not available. Ping reply to 0x%02X failed.\n", dest);
			return;
		} else {
			ping_packet.timestamp_back = time;
		}

		// Create a LowNet frame to send the ping packet
		lownet_frame_t ping_frame;
		ping_frame.source = lownet_get_device_id();
	    ping_frame.destination = dest;
	    ping_frame.protocol = LOWNET_PROTOCOL_PING;
	    ping_frame.length = sizeof(ping_packet);
	    memcpy(ping_frame.payload, &ping_packet, sizeof(ping_packet)); // Copy the ping packet to the frame payload.

	    // Send the frame
	    lownet_send(&ping_frame);
		printf("Ping received from 0x%02X. Ping reply sent.\n", dest);
	}


}