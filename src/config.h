#pragma once

#include <cfg_items.h>

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_mp_b;
    const CFG_HDR cfghdr_mp_0;
    char mp_csm_addr[16];
#ifdef NEWSGOLD
#ifndef ELKA
    const CFG_HDR cfghdr_mp_1;
    int detect_ims_700;
#endif
#endif
    const CFG_HDR cfghdr_mp_e;

    const CFG_HDR cfghdr_i_b;
    const CFG_HDR cfghdr_i_0;
    int enable_illumination;
    const CFG_HDR cfghdr_i_1;
    int override_brightness;
    const CFG_HDR cfghdr_i_e;
} CONFIG;
#pragma pack(pop)

extern CONFIG CFG;
extern char CFG_PATH[];

void InitConfig();
