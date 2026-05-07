#pragma once

#include <cfg_items.h>

enum {
    CFG_CLOCK_TYPE_BUILT_IN_DIGITAL,
    CFG_CLOCK_TYPE_PNG_DIGITAL,
};

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_c_b;
    const CFG_HDR cfghdr_c_0;
    int clock_type;
    const CFG_CBOX_ITEM cfgcbox_c_0[2];
    const CFG_HDR cfghdr_c_bi_d_b;
    const CFG_HDR cfghdr_c_bi_d_0;
    int built_in_digital_icon_0;
    const CFG_HDR cfghdr_c_bi_d_e;
    const CFG_HDR cfghdr_c_png_d_b;
    const CFG_HDR cfghdr_c_png_d_0;
    char png_digital_digit_bg_path[128];
    const CFG_HDR cfghdr_c_png_d_1;
    char png_digital_digits_path[128];
    const CFG_HDR cfghdr_c_png_d_2;
    int png_digital_offset_x;
    const CFG_HDR cfghdr_c_cp_d_e;
    const CFG_HDR cfghdr_c_e;
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
