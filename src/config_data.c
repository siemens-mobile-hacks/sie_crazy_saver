#include <cfg_items.h>

__CFG_BEGIN(0)

__root CFG_HDR cfghdr_0 = {CFG_STR_WIN1251, "MP CSM addr", 1, 8};
#ifdef NEWSGOLD
    #ifdef ELKA
        __root char CFG_MP_CSM_ADDR[9] = "A087109C";
    #else
        __root char CFG_MP_CSM_ADDR[9] = "A068ED54";
    #endif
#else
    __root char CFG_MP_CSM_ADDR[9] = "A0A29E70";
#endif

__root CFG_HDR cfghdr_1 = {CFG_CHECKBOX, "Enable MP illumination", 0, 2};
__root int CFG_ENABLE_ILLUMINATION = 1;

__root CFG_HDR cfghdr_2 = {CFG_UINT, "Font size clock", 0, 100};
#ifdef ELKA
    __root unsigned int CFG_FONT_SIZE_CLOCK = 100;
#else
    __root unsigned int CFG_FONT_SIZE_CLOCK = 52;
#endif

__root CFG_HDR cfghdr_3 = {CFG_UINT, "Font size track", 0, 100};
#ifdef ELKA
    __root unsigned int CFG_FONT_SIZE_TRACK = 28;
#else
    __root unsigned int CFG_FONT_SIZE_TRACK = 18;
#endif

__root CFG_HDR cfghdr_4 = {CFG_UINT, "Font size clock 2", 0, 100};
#ifdef ELKA
    __root unsigned int CFG_FONT_SIZE_CLOCK2 = 32;
#else
    __root unsigned int CFG_FONT_SIZE_CLOCK2 = 22;
#endif

__CFG_END(0)
