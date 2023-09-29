
#include <stddef.h>

#include "lownet_util.h"

static const lownet_identifier_t device_table[] = 
{
	// Reserved -- dead identifier.
	{{0x00,0x00,0x00,0x00,0x00,0x00}, 0},

	{{0x24,0x0a,0xc4,0x60,0x98,0xb4}, 0x10 },
	{{0x24,0x0a,0xc4,0x61,0x17,0x50}, 0x11 },
	{{0x24,0x0a,0xc4,0x60,0xa4,0xf8}, 0x12 },
	{{0x24,0x0a,0xc4,0x60,0xbb,0x78}, 0x13 },
	{{0x24,0x6f,0x28,0x7b,0x26,0x30}, 0x14 },
	{{0x24,0x6f,0x28,0x7b,0x37,0x44}, 0x15 },
	{{0x24,0x6f,0x28,0x7b,0x0f,0x38}, 0x16 },
	{{0x24,0x0a,0xc4,0x60,0xb0,0x48}, 0x17 },
	{{0x24,0x0a,0xc4,0x60,0xa4,0xd0}, 0x18 },
	{{0x24,0x6f,0x28,0x7b,0x3f,0x74}, 0x19 },
	{{0x24,0x0a,0xc4,0x60,0x9c,0xa0}, 0x1A },
	{{0x24,0x62,0xab,0xe1,0x26,0x7c}, 0x1B },
	{{0x24,0x0a,0xc4,0x60,0x99,0xe0}, 0x1C },
	{{0x24,0x6f,0x28,0x7b,0x33,0x88}, 0x1D },
	{{0x24,0x62,0xab,0xf9,0x5f,0xf8}, 0x1E },
	{{0x24,0x0a,0xc4,0x60,0x9a,0x00}, 0x1F },
	{{0x24,0x0a,0xc4,0x60,0x98,0xa4}, 0x20 },
	{{0x24,0x0a,0xc4,0x60,0xa7,0xdc}, 0x21 },
	{{0x24,0x62,0xab,0xf9,0x21,0xb0}, 0x22 },
	{{0x24,0x0a,0xc4,0x61,0x04,0x38}, 0x23 },
	{{0x24,0x62,0xab,0xf8,0xe7,0xd4}, 0x24 },
	{{0x24,0x62,0xab,0xf9,0x40,0xc8}, 0x25 },
	{{0x24,0x0a,0xc4,0x61,0x1f,0xfc}, 0x26 },
	{{0x24,0x62,0xab,0xf9,0x7b,0xa0}, 0x27 },
	{{0x24,0x0a,0xc4,0x61,0x01,0xe8}, 0x28 },
	{{0x24,0x0a,0xc4,0x61,0x04,0x20}, 0x29 },
	{{0x24,0x0a,0xc4,0x61,0x04,0x54}, 0x2A },
	{{0x24,0x62,0xab,0xf9,0x01,0xe8}, 0x2B },
	{{0x24,0x62,0xab,0xf8,0xfd,0xe4}, 0x2C },
	{{0x24,0x62,0xab,0xf9,0x72,0x64}, 0x2D },
	{{0x24,0x62,0xab,0xf9,0x16,0x64}, 0x2E },
	{{0x24,0x62,0xab,0xe1,0x12,0x38}, 0x2F },
	{{0x30,0xae,0xa4,0xf5,0x6d,0x04}, 0x30 },
	{{0x24,0x6f,0x28,0x7c,0xeb,0x58}, 0x31 },
	{{0x24,0x6f,0x28,0x7c,0xec,0xdc}, 0x32 },
	{{0x24,0x62,0xab,0xf8,0xe7,0x50}, 0x33 },
	{{0x24,0x0a,0xc4,0x60,0xa5,0xb0}, 0x34 },
	{{0x24,0x62,0xab,0xf9,0x12,0x28}, 0x35 },
	{{0x24,0x0a,0x00,0x00,0x00,0x00}, 0x36 },
	{{0x24,0x62,0xab,0xe1,0x34,0xe0}, 0x37 },
	{{0x24,0x0a,0xc4,0x61,0x19,0xc8}, 0x38 },
	{{0x24,0x62,0xab,0xf8,0xe7,0x44}, 0x39 },
	{{0x24,0x62,0xab,0xf9,0x50,0x98}, 0x3A },
	{{0x24,0x0a,0xc4,0x60,0xa5,0xfc}, 0x3B },

	// Extra devices, David.
	{{0x24,0x0a,0xc4,0x61,0x30,0x50}, 0xE0},

	// Deebugging device, Groska.
	{{0x24,0x0a,0xc4,0x61,0x04,0x00}, 0xF0},

	// Broadcast identifier.
	{{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, 0xFF}
};

// Lookup identifier by node id.
lownet_identifier_t lownet_lookup(uint8_t id) {
	size_t size = sizeof(device_table) / sizeof(device_table[0]);
	for (int i = 0; i < size; ++i) {
		if (device_table[i].node == id) {
			return device_table[i];
		}
	}
	return device_table[0];
}

// Lookup identifier by mac address.
lownet_identifier_t lownet_lookup_mac(const uint8_t* mac) {
	size_t size = sizeof(device_table) / sizeof(device_table[0]);
	for (int i = 0; i < size; ++i) {
		int match = 1;
		for (int j = 0; j < 6; ++j) {
			if (device_table[i].mac[j] != mac[j]) {
				match = 0;
			}
		}
		if (match) {
			return device_table[i];
		}
	}
	return device_table[0];
}


uint32_t lownet_crc(const lownet_frame_t* frame) {
	uint32_t reg = 0x00777777;	// Shift register initial vector.
	static const uint32_t poly = 0x1800463ul;  // G(x)

    void process_byte( uint8_t b )
    {
        for(int i=0; i<8; i++)
        {
            reg = (reg<<1) | (b&1);
            b   = b>>1;
            if ( reg & 0x1000000ul )
                reg = (reg^poly);     // take mod G(x)
        }
    }
    
	const uint8_t* iter = (const uint8_t*)frame;
	int len = LOWNET_FRAME_SIZE - LOWNET_CRC_SIZE;
    for(int i=0; i<len; i++)
        process_byte( iter[i] );
    return reg;
}