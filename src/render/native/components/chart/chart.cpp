#include "./chart.hpp"

namespace {

#if LV_USE_SCALE
lv_obj_t * ensureScale(lv_obj_t * chart_instance, lv_obj_t ** scale, lv_scale_mode_t mode, lv_align_t align) {
    if (*scale == nullptr) {
        *scale = lv_scale_create(chart_instance);
        lv_obj_remove_flag(*scale, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(*scale, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_EVENT_BUBBLE));
        lv_scale_set_mode(*scale, mode);
    }
    lv_obj_align(*scale, align, 0, 0);
    return *scale;
}

void configureAxisScale(
    lv_obj_t * scale,
    int32_t major_len,
    int32_t minor_len,
    int32_t major_num,
    int32_t minor_num,
    int32_t draw_size,
    bool horizontal
) {
    if (scale == nullptr) return;

    if (horizontal) {
        lv_obj_set_width(scale, lv_pct(100));
        lv_obj_set_height(scale, draw_size > 0 ? draw_size : 25);
    } else {
        lv_obj_set_width(scale, draw_size > 0 ? draw_size : 25);
        lv_obj_set_height(scale, lv_pct(100));
    }

    if (major_num > 0) {
        lv_scale_set_total_tick_count(scale, (uint32_t)major_num);
    }
    if (minor_num > 0) {
        lv_scale_set_major_tick_every(scale, (uint32_t)minor_num);
    }

    (void)major_len;
    (void)minor_len;
}

void applyScaleLabels(
    lv_obj_t * scale,
    std::vector<std::string> & labels,
    std::vector<const char *> & cstrs
) {
    if (scale == nullptr || labels.empty()) return;

    cstrs.clear();
    cstrs.reserve(labels.size() + 1);
    for (auto & label : labels) {
        cstrs.push_back(label.c_str());
    }
    cstrs.push_back(nullptr);
    lv_scale_set_text_src(scale, cstrs.data());
}
#endif

}  // namespace

Chart::Chart(std::string uid, lv_obj_t* parent): BasicComponent(uid) {
    this->type = COMP_TYPE_CHART;

    this->uid = uid;
    this->instance = lv_chart_create(parent != nullptr ? parent : GetWindowInstance());

    lv_group_add_obj(lv_group_get_default(), this->instance);

    lv_obj_add_flag(this->instance, static_cast<lv_obj_flag_t>(LV_OBJ_FLAG_EVENT_BUBBLE | LV_OBJ_FLAG_CLICK_FOCUSABLE));
    lv_obj_remove_flag(this->instance, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_user_data(this->instance, this);
    this->initStyle(LV_PART_MAIN);
};

void Chart::setType (int32_t type) {
    lv_chart_set_type(this->instance, static_cast<lv_chart_type_t>(type));
};

void Chart::setDivLineCount (int32_t hdiv, int32_t vdiv) {
    lv_chart_set_div_line_count(this->instance, hdiv, vdiv);
};

void Chart::setLeftAxisOption (
    int32_t major_len,
    int32_t minor_len,
    int32_t major_num,
    int32_t minor_num,
    int32_t draw_size
  ) {
#if LV_USE_SCALE
    lv_obj_t * scale = ensureScale(this->instance, &scale_left, LV_SCALE_MODE_VERTICAL_LEFT, LV_ALIGN_OUT_LEFT_MID);
    configureAxisScale(scale, major_len, minor_len, major_num, minor_num, draw_size, false);
    applyScaleLabels(scale, left_axis_labels, left_axis_label_cstrs);
#else
    (void)major_len;
    (void)minor_len;
    (void)major_num;
    (void)minor_num;
    (void)draw_size;
#endif
};

void Chart::setRightAxisOption (
    int32_t major_len,
    int32_t minor_len,
    int32_t major_num,
    int32_t minor_num,
    int32_t draw_size
  ) {
#if LV_USE_SCALE
    lv_obj_t * scale = ensureScale(this->instance, &scale_right, LV_SCALE_MODE_VERTICAL_RIGHT, LV_ALIGN_OUT_RIGHT_MID);
    configureAxisScale(scale, major_len, minor_len, major_num, minor_num, draw_size, false);
    applyScaleLabels(scale, right_axis_labels, right_axis_label_cstrs);
#else
    (void)major_len;
    (void)minor_len;
    (void)major_num;
    (void)minor_num;
    (void)draw_size;
#endif
};

void Chart::setTopAxisOption (
    int32_t major_len,
    int32_t minor_len,
    int32_t major_num,
    int32_t minor_num,
    int32_t draw_size
  ) {
#if LV_USE_SCALE
    lv_obj_t * scale = ensureScale(this->instance, &scale_top, LV_SCALE_MODE_HORIZONTAL_TOP, LV_ALIGN_OUT_TOP_MID);
    configureAxisScale(scale, major_len, minor_len, major_num, minor_num, draw_size, true);
    applyScaleLabels(scale, top_axis_labels, top_axis_label_cstrs);
#else
    (void)major_len;
    (void)minor_len;
    (void)major_num;
    (void)minor_num;
    (void)draw_size;
#endif
};

void Chart::setBottomAxisOption (
    int32_t major_len,
    int32_t minor_len,
    int32_t major_num,
    int32_t minor_num,
    int32_t draw_size
  ) {
#if LV_USE_SCALE
    lv_obj_t * scale = ensureScale(this->instance, &scale_bottom, LV_SCALE_MODE_HORIZONTAL_BOTTOM, LV_ALIGN_OUT_BOTTOM_MID);
    configureAxisScale(scale, major_len, minor_len, major_num, minor_num, draw_size, true);
    lv_obj_set_style_pad_hor(scale, lv_chart_get_first_point_center_offset(this->instance), 0);
    applyScaleLabels(scale, bottom_axis_labels, bottom_axis_label_cstrs);
#else
    (void)major_len;
    (void)minor_len;
    (void)major_num;
    (void)minor_num;
    (void)draw_size;
#endif
};

void Chart::setLeftAxisData (std::vector<axis_data>& data) {
    int32_t i, j, color;
    axis_data item;

    for (i=0; i<this->left_axis.size(); i++) {
        lv_chart_remove_series(this->instance, this->left_axis[i]);
    }
    this->left_axis.clear();

    for (i=0; i<data.size(); i++) {
        color = data[i].color;
        if (color == -1) {
            this->left_axis.push_back(lv_chart_add_series(this->instance, lv_theme_get_color_primary(this->instance), LV_CHART_AXIS_PRIMARY_Y));
        } else {
            this->left_axis.push_back(lv_chart_add_series(this->instance, lv_color_hex(color), LV_CHART_AXIS_PRIMARY_Y));
        }
    }

    for (i=0; i<data.size(); i++) {
        item = data[i];
        for (j=0; j<item.data.size(); j++) {
            lv_chart_set_value_by_id(this->instance, this->left_axis[i], j, item.data[j]);
        }
    }
};

void Chart::setRightAxisData (std::vector<axis_data>& data) {
    int32_t i, j, color;
    axis_data item;

    for (i=0; i<this->right_axis.size(); i++) {
        lv_chart_remove_series(this->instance, this->right_axis[i]);
    }
    this->right_axis.clear();

    for (i=0; i<data.size(); i++) {
        color = data[i].color;
        if (color == -1) {
            this->right_axis.push_back(lv_chart_add_series(this->instance, lv_theme_get_color_primary(this->instance), LV_CHART_AXIS_PRIMARY_Y));
        } else {
            this->right_axis.push_back(lv_chart_add_series(this->instance, lv_color_hex(color), LV_CHART_AXIS_SECONDARY_Y));
        }
    }

    for (i=0; i<data.size(); i++) {
        item = data[i];
        for (j=0; j<item.data.size(); j++) {
            lv_chart_set_value_by_id(this->instance, this->right_axis[i], j, item.data[j]);
        }
    }
};

// void Chart::setTopAxisData (std::vector<axis_data>& data) {
//     if (this->top_axis == nullptr) {
//         this->top_axis = lv_chart_add_series(this->instance, lv_theme_get_color_primary(this->instance), LV_CHART_AXIS_SECONDARY_X);
//     }
//     int32_t i;
//     for (i=0; i<data.size(); i++) {
//         this->top_axis->y_points[i] = (lv_coord_t)data[i];
//     }
// };

// void Chart::setBottomAxisData (std::vector<axis_data>& data) {
//     int32_t i;
//     if (this->bottom_axis == nullptr) {
//         this->bottom_axis = lv_chart_add_series(this->instance, lv_theme_get_color_primary(this->instance), LV_CHART_AXIS_PRIMARY_X);
//     }
//     for (i=0; i<data.size(); i++) {
//         this->bottom_axis->y_points[i] = (lv_coord_t)data[i];
//     }
// };

void Chart::setPointNum (int32_t num) {
    lv_chart_set_point_count(this->instance, (uint16_t)num);
#if LV_USE_SCALE
    if (scale_bottom != nullptr) {
        lv_obj_set_style_pad_hor(scale_bottom, lv_chart_get_first_point_center_offset(this->instance), 0);
    }
#endif
};

void Chart::setLeftAxisLabels (std::vector<std::string>& labels) {
    this->left_axis_labels = labels;
#if LV_USE_SCALE
    applyScaleLabels(scale_left, left_axis_labels, left_axis_label_cstrs);
#endif
};

void Chart::setRightAxisLabels (std::vector<std::string>& labels) {
    this->right_axis_labels = labels;
#if LV_USE_SCALE
    applyScaleLabels(scale_right, right_axis_labels, right_axis_label_cstrs);
#endif
};

void Chart::setTopAxisLabels (std::vector<std::string>& labels) {
    this->top_axis_labels = labels;
#if LV_USE_SCALE
    applyScaleLabels(scale_top, top_axis_labels, top_axis_label_cstrs);
#endif
};

void Chart::setBottomAxisLabels (std::vector<std::string>& labels) {
    this->bottom_axis_labels = labels;
#if LV_USE_SCALE
    applyScaleLabels(scale_bottom, bottom_axis_labels, bottom_axis_label_cstrs);
#endif
};

void Chart::setLeftAxisRange (int32_t min, int32_t max) {
    lv_chart_set_range(this->instance, LV_CHART_AXIS_PRIMARY_Y, min, max);
};

void Chart::setRightAxisRange (int32_t min, int32_t max) {
    lv_chart_set_range(this->instance, LV_CHART_AXIS_SECONDARY_Y, min, max);
};

void Chart::setTopAxisRange (int32_t min, int32_t max) {
    lv_chart_set_range(this->instance, LV_CHART_AXIS_SECONDARY_X, min, max);
};

void Chart::setBottomAxisRange (int32_t min, int32_t max) {
    lv_chart_set_range(this->instance, LV_CHART_AXIS_PRIMARY_X, min, max);
};

void Chart::setScatterData (std::vector<axis_data>& data) {
    int32_t i, j, color;
    axis_data item;

    for (i=0; i<this->left_axis.size(); i++) {
        lv_chart_remove_series(this->instance, this->left_axis[i]);
    }
    this->left_axis.clear();

    for (i=0; i<data.size(); i++) {
        color = data[i].color;
        if (color == -1) {
            this->left_axis.push_back(lv_chart_add_series(this->instance, lv_theme_get_color_primary(this->instance), LV_CHART_AXIS_PRIMARY_Y));
        } else {
            this->left_axis.push_back(lv_chart_add_series(this->instance, lv_color_hex(color), LV_CHART_AXIS_PRIMARY_Y));
        }
    }

    for (i=0; i<data.size(); i++) {
        item = data[i];
        for (j=0; j<item.data.size(); j+=2) {
            lv_chart_set_value_by_id2(this->instance, this->left_axis[i], j / 2, item.data[j], item.data[j + 1]);
        }
    }
};
