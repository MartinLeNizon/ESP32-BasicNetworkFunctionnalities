#ifndef GUARD_APP_CHAT_H
#define GUARD_APP_CHAT_H

#include <stdint.h>

#include "lownet.h"

int message_is_printable(const char* message, const uint8_t length);

void chat_receive(const lownet_frame_t* frame);

void chat_shout(const char* message);

void chat_tell(const char* message, uint8_t destination);

#endif