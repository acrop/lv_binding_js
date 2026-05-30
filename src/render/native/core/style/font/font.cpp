#include "font.hpp"

#include <cstdio>
#include <cstdint>

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

static const int builtin_font_size_list[BUILTIN_FONT_COUNT] = {
    8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
};

void InitBuiltinFonts(const char* cjk_ttf_path) {
#if LV_USE_TINY_TTF
    FILE* f = fopen(cjk_ttf_path, "rb");
    if (f == nullptr) {
        LV_LOG_WARN("CJK font '%s' could not be opened; Chinese will not render", cjk_ttf_path);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) {
        fclose(f);
        return;
    }

    /* Kept alive for the whole program: tiny_ttf stores a pointer to this data
     * (it does not copy it), and every size shares the same buffer. */
    uint8_t* data = static_cast<uint8_t*>(lv_malloc(size));
    if (data == nullptr) {
        fclose(f);
        return;
    }
    size_t read = fread(data, 1, size, f);
    fclose(f);
    if (read != static_cast<size_t>(size)) {
        lv_free(data);
        return;
    }

    for (int i = 0; i < BUILTIN_FONT_COUNT; i++) {
        lv_font_t* cjk = lv_tiny_ttf_create_data(data, size, builtin_font_size_list[i]);
        if (cjk != nullptr) {
            builtin_font_list[i].fallback = cjk;
        }
    }
#else
    (void)cjk_ttf_path;
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
