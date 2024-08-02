#pragma once


#include "lv_bindings_js.h"
#include "native/core/style/font/font.hpp"

static lv_theme_t *theme_default = NULL;

void NativeThemeInit (JSContext* ctx, JSValue& ns);