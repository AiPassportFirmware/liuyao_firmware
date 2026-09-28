// main/liuyao_pages_flow.c —— 六爻应用流程页:封面/问事分类/视角/日期/起卦方式。
// 全新设计的竖屏 UI:墨底宣纸字,三键导航(上下选择,OK 确认,长按返回)。
#include <stdio.h>
#include <string.h>

#include "bsp_button.h"
#include "liuyao_app_internal.h"
#include "liuyao_calendar.h"
#include "liuyao_data.h"
#include "liuyao_theme.h"

// ---------------------------------------------------------------------------
// 封面
// ---------------------------------------------------------------------------
// 动画回调:透明度渐变(呼吸/闪烁)。对象删除时 LVGL 自动清理绑定动画。
static void home_opa_anim(void *var, int32_t value) {
    lv_obj_set_style_opa((lv_obj_t *)var, value, 0);
}

static void home_build(struct liyao_app_s *app) {
    app->screen = liuyao_page_create(NULL);

    // 太极的舞台:鎏金光晕 + 呼吸金环。
    lv_obj_t *halo = lv_obj_create(app->screen);
    lv_obj_remove_style_all(halo);
    lv_obj_set_size(halo, 148, 148);
    lv_obj_set_pos(halo, 120 - 74, 108 - 74);
    lv_obj_set_style_radius(halo, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(halo, lv_color_hex(LY_COLOR_GOLD), 0);
    lv_obj_set_style_bg_opa(halo, LV_OPA_10, 0);
    lv_obj_t *breathe = liuyao_ring_create(app->screen, 120, 108, 64, 1,
                                           LY_COLOR_GOLD, LV_OPA_40);
    liuyao_taiji_create(app->screen, 120, 108, 52);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, breathe);
    lv_anim_set_exec_cb(&a, home_opa_anim);
    lv_anim_set_values(&a, LV_OPA_20, LV_OPA_70);
    lv_anim_set_time(&a, 1500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    lv_obj_t *title = lv_label_create(app->screen);
    lv_obj_set_style_text_font(title, &liuyao_font_48, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(LY_COLOR_PAPER), 0);
    lv_label_set_text(title, "六爻");
    // 用对齐而非固定 x:字宽随字距变化,对齐才能保证与太极同轴。
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 176);

    // 鎏金分隔线:两端金点,中间细线。
    liuyao_hairline(app->screen, 64, 234, 112);
    liuyao_rect_create(app->screen, 60, 233, 3, 3, LY_COLOR_GOLD);
    liuyao_rect_create(app->screen, 177, 233, 3, 3, LY_COLOR_GOLD);

    lv_obj_t *subtitle = lv_label_create(app->screen);
    lv_obj_set_style_text_font(subtitle, &liuyao_font_16, 0);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(LY_COLOR_PAPER_DIM), 0);
    lv_label_set_text(subtitle, "文王纳甲 · 铜钱摇卦");
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 244);

    lv_obj_t *hint = liuyao_hint_create(app->screen, 276, "按 OK 开始起卦");
    lv_obj_set_style_text_color(hint, lv_color_hex(LY_COLOR_GOLD), 0);
    lv_anim_init(&a);
    lv_anim_set_var(&a, hint);
    lv_anim_set_exec_cb(&a, home_opa_anim);
    lv_anim_set_values(&a, LV_OPA_50, LV_OPA_COVER);
    lv_anim_set_time(&a, 900);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void home_key(struct liyao_app_s *app, bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (btn == BSP_BTN_OK && ev == BSP_BTN_CLICK) {
        ly_app_goto(app, LY_STATE_CATEGORY);
    }
}

// ---------------------------------------------------------------------------
// 问事分类(3×3 网格)
// ---------------------------------------------------------------------------
static void grid_refresh(struct liyao_app_s *app, const ly_grid_widgets_t *grid,
                         int count) {
    for (int i = 0; i < count; i++) {
        ly_style_option(grid->cells[i], i == app->sel);
    }
}

static void category_build(struct liyao_app_s *app) {
    app->screen = liuyao_page_create("问何事");
    for (int i = 0; i < LIUYAO_CAT_COUNT; i++) {
        lv_obj_t *panel = lv_obj_create(app->screen);
        lv_obj_remove_style_all(panel);
        int col = i % 3;
        int row = i / 3;
        // 网格总宽 3*70+2*4=218,左右各留 11px,与四角角饰留白一致。
        lv_obj_set_pos(panel, 11 + col * 74, 44 + row * 62);
        lv_obj_set_size(panel, 70, 54);
        lv_obj_t *label = lv_label_create(panel);
        lv_obj_set_style_text_font(label, &liuyao_font_24, 0);
        lv_obj_center(label);
        lv_label_set_text(label, liuyao_category_label(i));
        app->category.cells[i] = panel;
    }
    liuyao_hint_create(app->screen, 238, "上下选择 · OK 确认");
    app->sel = 0;
    grid_refresh(app, &app->category, LIUYAO_CAT_COUNT);
}

static void category_key(struct liyao_app_s *app, bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (btn == BSP_BTN_UP && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + LIUYAO_CAT_COUNT - 1) % LIUYAO_CAT_COUNT;
        grid_refresh(app, &app->category, LIUYAO_CAT_COUNT);
        liuyao_pop(app->category.cells[app->sel]);
    } else if (btn == BSP_BTN_DOWN && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + 1) % LIUYAO_CAT_COUNT;
        grid_refresh(app, &app->category, LIUYAO_CAT_COUNT);
        liuyao_pop(app->category.cells[app->sel]);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_CLICK) {
        app->casting.category = (liuyao_category_t)app->sel;
        ly_app_goto(app, app->casting.category == LIUYAO_CAT_RELATION
                             ? LY_STATE_PERSPECTIVE
                             : LY_STATE_DATE);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) {
        ly_app_goto(app, LY_STATE_HOME);
    }
}

// ---------------------------------------------------------------------------
// 感情视角(3 项纵向列表)
// ---------------------------------------------------------------------------
static void perspective_build(struct liyao_app_s *app) {
    app->screen = liuyao_page_create("感情视角");
    const int count = LIUYAO_PERSP_FEMALE + 1;
    for (int i = 0; i < count; i++) {
        lv_obj_t *panel = lv_obj_create(app->screen);
        lv_obj_remove_style_all(panel);
        // 三行总高 3*44+2*8=140,在标题线(34)与提示(228)之间居中,
        // 避免下方压出一大段空档。
        lv_obj_set_pos(panel, 30, 62 + i * 52);
        lv_obj_set_size(panel, 180, 44);
        lv_obj_t *label = lv_label_create(panel);
        lv_obj_set_style_text_font(label, &liuyao_font_24, 0);
        lv_obj_center(label);
        lv_label_set_text(label, liuyao_perspective_label(i));
        app->perspective.cells[i] = panel;
    }
    liuyao_hint_create(app->screen, 228, "男问妻财 · 女问官鬼");
    app->sel = LIUYAO_PERSP_UNSPECIFIED;
    grid_refresh(app, &app->perspective, LIUYAO_PERSP_FEMALE + 1);
}

static void perspective_key(struct liyao_app_s *app, bsp_btn_t btn,
                            bsp_btn_ev_t ev) {
    const int count = LIUYAO_PERSP_FEMALE + 1;
    if (btn == BSP_BTN_UP && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + count - 1) % count;
        grid_refresh(app, &app->perspective, count);
        liuyao_pop(app->perspective.cells[app->sel]);
    } else if (btn == BSP_BTN_DOWN && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + 1) % count;
        grid_refresh(app, &app->perspective, count);
        liuyao_pop(app->perspective.cells[app->sel]);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_CLICK) {
        app->casting.perspective = (liuyao_perspective_t)app->sel;
        ly_app_goto(app, LY_STATE_DATE);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) {
        ly_app_goto(app, LY_STATE_CATEGORY);
    }
}

// ---------------------------------------------------------------------------
// 起卦日期(年/月/日/时 四字段)
// ---------------------------------------------------------------------------
static const int k_date_min[LY_DATE_FIELD_COUNT] = {
    LIUYAO_DATE_MIN_YEAR, 1, 1, 0};
static const int k_date_max[LY_DATE_FIELD_COUNT] = {
    LIUYAO_DATE_MAX_YEAR, 12, 31, 23};
static const char *const k_date_title[LY_DATE_FIELD_COUNT] = {"年", "月", "日", "时"};

#define LY_METHOD_COUNT 2  // 起卦方式选项数(摇卦/手动录爻)

static void date_refresh(struct liyao_app_s *app) {
    int values[LY_DATE_FIELD_COUNT] = {app->year, app->month, app->day, app->hour};
    for (int i = 0; i < LY_DATE_FIELD_COUNT; i++) {
        bool active = i == app->date_field;
        lv_label_set_text_fmt(app->date.values[i], "%d", values[i]);
        lv_obj_set_style_text_color(app->date.values[i],
            lv_color_hex(active ? LY_COLOR_GOLD : LY_COLOR_PAPER), 0);
        lv_obj_set_style_text_color(app->date.titles[i],
            lv_color_hex(active ? LY_COLOR_GOLD : LY_COLOR_PAPER_DIM), 0);
        // 选中字段下的鎏金短横标记。
        lv_obj_set_style_bg_color(app->date.marks[i],
            lv_color_hex(active ? LY_COLOR_GOLD : LY_COLOR_PANEL_2), 0);
        lv_obj_set_style_bg_opa(app->date.marks[i],
            active ? LV_OPA_COVER : LV_OPA_40, 0);
    }
}

static void date_clamp(struct liyao_app_s *app) {
    if (app->year < k_date_min[0]) app->year = k_date_max[0];
    if (app->year > k_date_max[0]) app->year = k_date_min[0];
    if (app->month < 1) app->month = 12;
    if (app->month > 12) app->month = 1;
    int dim = ly_days_in_month_clamped(app->year, app->month, app->day);
    if (app->day > dim) app->day = dim;
    if (app->day < 1) app->day = 1;
    if (app->hour < 0) app->hour = 23;
    if (app->hour > 23) app->hour = 0;
}

static void date_build(struct liyao_app_s *app) {
    app->screen = liuyao_page_create("起卦时间");
    // 四列等宽(54px)均分,年(4 位)到时(1-2 位)都以列中心对齐,
    // 避免原来按内容宽度排布造成的疏密不一。
    static const int k_field_x[LY_DATE_FIELD_COUNT] = {12, 66, 120, 174};
    for (int i = 0; i < LY_DATE_FIELD_COUNT; i++) {
        lv_obj_t *title = lv_label_create(app->screen);
        lv_obj_set_style_text_font(title, &liuyao_font_24, 0);
        lv_obj_set_width(title, 54);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(title, k_field_x[i], 92);
        lv_label_set_text(title, k_date_title[i]);
        app->date.titles[i] = title;
        lv_obj_t *value = lv_label_create(app->screen);
        lv_obj_set_style_text_font(value, &liuyao_font_24, 0);
        lv_obj_set_width(value, 54);
        lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_pos(value, k_field_x[i], 124);
        app->date.values[i] = value;
        // 选中标记(24px)也在列内居中。
        app->date.marks[i] = liuyao_rect_create(app->screen, k_field_x[i] + 15,
                                                164, 24, 3, LY_COLOR_PANEL_2);
        lv_obj_set_style_bg_opa(app->date.marks[i], LV_OPA_40, 0);
    }
    liuyao_hint_create(app->screen, 214, "上下调整 · OK 下一项");
    app->date_field = 0;
    date_refresh(app);
}

static void date_key(struct liyao_app_s *app, bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (btn == BSP_BTN_UP && ev == BSP_BTN_CLICK) {
        int *fields[LY_DATE_FIELD_COUNT] = {&app->year, &app->month, &app->day,
                                            &app->hour};
        *fields[app->date_field] += 1;
        date_clamp(app);
        date_refresh(app);
    } else if (btn == BSP_BTN_DOWN && ev == BSP_BTN_CLICK) {
        int *fields[LY_DATE_FIELD_COUNT] = {&app->year, &app->month, &app->day,
                                            &app->hour};
        *fields[app->date_field] -= 1;
        date_clamp(app);
        date_refresh(app);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_CLICK) {
        app->date_field++;
        if (app->date_field >= LY_DATE_FIELD_COUNT) {
            app->date_field = 0;
            ly_app_save_last_date(app);
            ly_app_goto(app, LY_STATE_METHOD);
        } else {
            date_refresh(app);
        }
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) {
        // 返回上层时也存盘:UP/DOWN 已改过内存里的日期,不存就会与
        // NVS 中的"上次日期"不一致,下次进页又显示旧值。
        ly_app_save_last_date(app);
        ly_app_goto(app, LY_STATE_CATEGORY);
    }
}

// ---------------------------------------------------------------------------
// 起卦方式(2 项)
// ---------------------------------------------------------------------------
static void method_build(struct liyao_app_s *app) {
    app->screen = liuyao_page_create("起卦方式");
    static const char *const k_options[LY_METHOD_COUNT] = {"摇卦(自动)", "手动录爻"};
    for (int i = 0; i < LY_METHOD_COUNT; i++) {
        lv_obj_t *panel = lv_obj_create(app->screen);
        lv_obj_remove_style_all(panel);
        // 两行总高 2*46+10=102,在标题线(34)与提示(214)之间居中。
        lv_obj_set_pos(panel, 30, 74 + i * 56);
        lv_obj_set_size(panel, 180, 46);
        lv_obj_t *label = lv_label_create(panel);
        lv_obj_set_style_text_font(label, &liuyao_font_24, 0);
        lv_obj_center(label);
        lv_label_set_text(label, k_options[i]);
        app->perspective.cells[i] = panel;  // 复用通用网格部件
    }
    liuyao_hint_create(app->screen, 214, "已有铜钱结果请选手动录爻");
    app->sel = 0;
    grid_refresh(app, &app->perspective, LY_METHOD_COUNT);
}

static void method_key(struct liyao_app_s *app, bsp_btn_t btn, bsp_btn_ev_t ev) {
    if (btn == BSP_BTN_UP && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + LY_METHOD_COUNT - 1) % LY_METHOD_COUNT;
        grid_refresh(app, &app->perspective, LY_METHOD_COUNT);
        liuyao_pop(app->perspective.cells[app->sel]);
    } else if (btn == BSP_BTN_DOWN && ev == BSP_BTN_CLICK) {
        app->sel = (app->sel + 1) % LY_METHOD_COUNT;
        grid_refresh(app, &app->perspective, LY_METHOD_COUNT);
        liuyao_pop(app->perspective.cells[app->sel]);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_CLICK) {
        ly_app_goto(app, app->sel == 0 ? LY_STATE_CASTING : LY_STATE_MANUAL);
    } else if (btn == BSP_BTN_OK && ev == BSP_BTN_LONG) {
        ly_app_goto(app, LY_STATE_DATE);
    }
}

static const ly_page_ops_t k_home_ops = {home_build, home_key, NULL};
static const ly_page_ops_t k_category_ops = {category_build, category_key, NULL};
static const ly_page_ops_t k_perspective_ops = {perspective_build, perspective_key,
                                                NULL};
static const ly_page_ops_t k_date_ops = {date_build, date_key, NULL};
static const ly_page_ops_t k_method_ops = {method_build, method_key, NULL};

const ly_page_ops_t *liuyao_page_ops_flow(ly_state_t state) {
    switch (state) {
        case LY_STATE_HOME: return &k_home_ops;
        case LY_STATE_CATEGORY: return &k_category_ops;
        case LY_STATE_PERSPECTIVE: return &k_perspective_ops;
        case LY_STATE_DATE: return &k_date_ops;
        case LY_STATE_METHOD: return &k_method_ops;
        default: return NULL;
    }
}
