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

	// ping_packet.timestamp_back remains empty

	ping_packet.origin = lownet_get_device_id();

	// Create a LowNet frame to send the ping packet
	lownet_frame_t ping_frame;
	ping_frame.source = ping_packet.origin;
    ping_frame.destination = node;
    ping_frame.protocol = LOWNET_PROTOCOL_PING;
    ping_frame.length = sizeof(ping_packet);
    memcpy(ping_frame.payload, &ping_packet, sizeof(ping_packet)); // Copy the ping packet to the frame payload.

    // Send the frame
    lownet_send(&ping_frame);
	printf("Pinging %u:\n", node);
}


void ping_receive(const lownet_frame_t* frame) {
	if (frame->length < sizeof(ping_packet_t)) {
		// Malformed frame.  Discard.
		return;
	}

	// Extract the ping packet from the frame payload
	ping_packet_t received_ping;
	memcpy(&received_ping, frame->payload, sizeof(ping_packet_t));

	// Calculate the round-trip time in milliseconds
	uint64_t round_trip_ms = (received_ping.timestamp_back.seconds - received_ping.timestamp_out.seconds) * 1000 + ((received_ping.timestamp_back.parts - received_ping.timestamp_out.parts) * 1000) / 256; // Border line case if dif > 65536 -> need uint64_t

	printf("Ping reply from %u: time: %llums\n", received_ping.origin, round_trip_ms);

	// Check if the round-trip time exceeds the maximum allowed time
	if (round_trip_ms > MAX_PING_MS) {
		printf("Round-trip time exceeds maximum allowed time.\n");
	}
}