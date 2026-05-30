#pragma once

#include "lv_bindings_js.h"

#include <vector>

#define BUILTIN_FONT_COUNT 21

extern lv_font_t builtin_font_list[BUILTIN_FONT_COUNT];

/* Attach the built-in SimSun CJK font as a fallback to every Montserrat font so
 * Chinese/CJK glyphs render even though the primary fonts are Latin-only.
 * Note: the built-in SimSun font only contains ~1000 curated glyphs, so
 * characters outside that set still show as placeholder boxes.
 * Safe to call once after lv_init(). */
void InitBuiltinFonts();

void CompSetFontSize (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj);

void CompSetFontSize1 (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj);
