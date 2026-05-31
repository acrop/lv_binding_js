#pragma once

#include "lvgl.h"

#if LV_USE_IME_PINYIN

#ifdef __cplusplus
extern "C" {
#endif

/* Custom Pinyin IME dictionary covering the common GB2312 simplified-Chinese
 * character set (matches the coverage of lv_font_msyh_16). It replaces LVGL's
 * built-in thesaurus, which only ships ~1000 traditional/CJK radicals and is
 * missing many common syllables (e.g. "lan"). The table is grouped by initial
 * letter and terminated with {NULL, NULL} as required by lv_ime_pinyin. */
extern const lv_pinyin_dict_t acrop_pinyin_dict[];

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_IME_PINYIN */
