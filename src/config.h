#pragma once

#include <cfg_items.h>

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_0;
    char mp_csm_addr[16];

    const CFG_HDR cfghdr_i_b;
    const CFG_HDR cfghdr_i_0;
    int enable_illumination;
    const CFG_HDR cfghdr_i_1;
    int override_brightness;
    const CFG_HDR cfghdr_i_e;

    const CFG_HDR cfghdr_2;
    int font_size_clock;

    const CFG_HDR cfghdr_3;
    int font_size_track;

    const CFG_HDR cfghdr_4;
    int font_size_clock2;
} CONFIG;
#pragma pack(pop)

extern CONFIG CFG;
extern char CFG_PATH[];

void InitConfig();
