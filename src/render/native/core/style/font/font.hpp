#pragma once

#include "lv_bindings_js.h"

#include <vector>

#define BUILTIN_FONT_COUNT 21

extern lv_font_t builtin_font_list[BUILTIN_FONT_COUNT];

/* Load a CJK TTF (e.g. a system font) once and attach a size-matched fallback
 * to every built-in Montserrat font, so Chinese/CJK glyphs render with full
 * coverage even though the primary fonts are Latin-only. The TTF data is read
 * a single time and shared across all sizes. Safe to call once after lv_init().
 * Falls back silently (Latin only) if the path can't be opened. */
void InitBuiltinFonts(const char* cjk_ttf_path);

void CompSetFontSize (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj);

void CompSetFontSize1 (lv_obj_t* comp, lv_style_t* style, JSContext* ctx, JSValue obj);
