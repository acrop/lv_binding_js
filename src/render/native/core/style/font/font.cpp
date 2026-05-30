#include "font.hpp"

lv_font_t builtin_font_list[BUILTIN_FONT_COUNT] = {
    lv_font_montserrat_8,
    lv_font_montserrat_10,
    lv_font_montserrat_12,
    lv_font_montserrat_14,
    lv_font_montserrat_16,
    lv_font_montserrat_18,
    lv_font_montserrat_20,
    lv_font_montserrat_22,
    lv_font_montserrat_24,
    lv_font_montserrat_26,
    lv_font_montserrat_28,
    lv_font_montserrat_30,
    lv_font_montserrat_32,
    lv_font_montserrat_34,
    lv_font_montserrat_36,
    lv_font_montserrat_38,
    lv_font_montserrat_40,
    lv_font_montserrat_42,
    lv_font_montserrat_44,
    lv_font_montserrat_46,
    lv_font_montserrat_48,
};

void InitBuiltinFonts() {
#if LV_FONT_SIMSUN_16_CJK
    /* Only the 16px SimSun (~1000 most-common CJK glyphs) is built into LVGL, so
     * all sizes fall back to it. CJK text therefore always renders at ~16px, and
     * characters outside that curated set will still show placeholder boxes. */
    for (int i = 0; i < BUILTIN_FONT_COUNT; i++) {
        builtin_font_list[i].fallback = &lv_font_simsun_16_cjk;
    }
#endif
};

void CompSetFontSize (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj) {
    int x;
    JS_ToInt32(ctx, &x, obj);

    lv_style_set_text_font(style, &builtin_font_list[x]);
};

void CompSetFontSize1 (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj) {
    int x;
    JS_ToInt32(ctx, &x, obj);

    lv_style_set_text_font(style, &builtin_font_list[x]);
};
