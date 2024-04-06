#include <cfg_items.h>

__CFG_BEGIN(0)

__root CFG_HDR cfghdr_0 = {CFG_STR_WIN1251, "MP CSM addr", 1, 8};
__root char CFG_MP_CSM_ADDR[9] = "A068ED54";

__root CFG_HDR cfghdr_1 = {CFG_CHECKBOX, "Enable MP illumination", 0, 2};
__root int CFG_ENABLE_MP_ILLUMINATION = 1;

__root CFG_HDR cfghdr_2 = {CFG_UINT, "Font size clock", 0, 100};
__root unsigned int CFG_FONT_SIZE_CLOCK = 52;

__root CFG_HDR cfghdr_3 = {CFG_UINT, "Font size track", 0, 100};
__root unsigned int CFG_FONT_SIZE_TRACK = 18;

__CFG_END(0)
