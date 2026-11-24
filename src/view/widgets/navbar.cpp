#include "navbar.h"

#include "asset_manager.h"
#include "bindings.h"
#include "linux_input.h"
#include "theme.h"
#include "ui_const.h"

namespace view::widgets {
namespace {

struct IconSpec {
    const char* text;
    lv_event_cb_t click_cb;
};

constexpr std::array<int32_t, 5> kNavButtonXOffsets = {30, 17, 2, -15, -30};

struct ShortcutSpec {
    const char* key;
    const char* action;
};

constexpr std::array<ShortcutSpec, 3> kConfirmShortcuts = {{
    {"ESC", "Cancel"},
    {"", ""},
    {"Enter", "Confirm"},
}};

constexpr std::array<ShortcutSpec, 3> kHomeShortcuts = {{
    {ICON_SELECT_ARROWS, "Select"},
    {"Enter", "OK"},
    {"ESC", "Exit"},
}};

}
NavBar::NavBar(lv_obj_t* parent, viewmodel::BaseViewModel& view_model, app::AssetManager& assets)
    : BaseWidgets(parent), view_model_(view_model), assets_(assets) {}

NavBar::~NavBar() {
    for (size_t i = 0; i < icon_buttons_.size(); ++i) {
        if (icon_buttons_[i]) {
            platform::unregister_nav_button(i, icon_buttons_[i]->root());
        }
    }

    if (page_observer_) {
        lv_observer_remove(page_observer_);
    }
    page_observer_ = nullptr;
    if (theme_observer_) {
        lv_observer_remove(theme_observer_);
        theme_observer_ = nullptr;
    }
}

void NavBar::build() {
    if (core_obj_) {
        return;
    }

    core_obj_ = lv_obj_create(parent_);
    lv_obj_remove_style_all(core_obj_);
    lv_obj_set_size(core_obj_, LV_PCT(100), kNavBarHeight);
    lv_obj_align(core_obj_, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_clear_flag(core_obj_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(core_obj_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(core_obj_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(core_obj_, 8, 0);
    lv_obj_set_style_pad_right(core_obj_, 8, 0);
    reactive::bind_theme(core_obj_, view_model_.dark_mode_subject(), reactive::ThemeRole::Bar);

    create_icon_buttons();
    create_shortcut_hints();
    page_observer_ = reactive::observe_obj(core_obj_, view_model_.current_page_subject(), update_icons_cb, this);
    theme_observer_ = reactive::observe_obj(core_obj_, view_model_.dark_mode_subject(), update_icons_cb, this);
    update_icon_buttons();
}

void NavBar::create_shortcut_hints() {
    shortcut_bar_ = lv_obj_create(core_obj_);
    lv_obj_remove_style_all(shortcut_bar_);
    lv_obj_set_size(shortcut_bar_, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(shortcut_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(shortcut_bar_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(shortcut_bar_, 10, 0);
    lv_obj_clear_flag(shortcut_bar_, LV_OBJ_FLAG_SCROLLABLE);

    shortcut_key_font_ = assets_.load_standard_font(14);
    if (!shortcut_key_font_) {
        shortcut_key_font_ = const_cast<lv_font_t*>(&lv_font_montserrat_14);
    }
    shortcut_icon_font_ = assets_.load_font("Phosphor-Fill.ttf", 16);
    const lv_font_t* text_font = assets_.load_standard_font(14, app::StandardFontWeight::Regular);
    if (!text_font) {
        text_font = &lv_font_montserrat_14;
    }

    for (size_t index = 0; index < kConfirmShortcuts.size(); ++index) {
        auto* item = lv_obj_create(shortcut_bar_);
        lv_obj_remove_style_all(item);
        lv_obj_set_size(item, LV_SIZE_CONTENT, LV_PCT(100));
        lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(item, 3, 0);
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

        auto* key = lv_label_create(item);
        lv_label_set_text(key, kConfirmShortcuts[index].key);
        lv_obj_set_style_text_font(key, shortcut_key_font_, 0);
        shortcut_key_labels_[index] = key;

        auto* label = lv_label_create(item);
        lv_label_set_text(label, kConfirmShortcuts[index].action);
        lv_obj_set_style_text_font(label, text_font, 0);
        reactive::bind_theme(label, view_model_.dark_mode_subject(), reactive::ThemeRole::Text);
    }
}

void NavBar::create_icon_buttons() {
    const auto light_color = view::palette(false).text;
    const auto dark_color = view::palette(true).text;
    icon_font_ = assets_.load_font("Phosphor-Fill.ttf", 22);

    for (size_t i = 0; i < icon_buttons_.size(); ++i) {
        icon_buttons_[i] = std::make_unique<IconButton>(core_obj_,
                                                        view_model_,
                                                        32,
                                                        24,
                                                        "",
                                                        icon_font_ ? icon_font_ : &lv_font_montserrat_14,
                                                        light_color,
                                                        dark_color,
                                                        nullptr,
                                                        &view_model_,
                                                        true);
        icon_buttons_[i]->build();
        lv_obj_set_style_translate_x(icon_buttons_[i]->root(), kNavButtonXOffsets[i], 0);
        platform::register_nav_button(i, icon_buttons_[i]->root());
    }
}

void NavBar::update_icon_buttons() {
    const bool is_confirm_page = view_model_.current_page() == model::AppPage::Confirm;
    const bool is_dark_mode = view_model_.is_dark_mode();
    const auto colors = view::palette(is_dark_mode);
    const char* theme_icon = is_dark_mode ? ICON_SUN : ICON_MOON;

    const std::array<IconSpec, 5> home_icons = {{
        {ICON_SIGN_OUT, request_quit_cb},
        {"", nullptr},
        {theme_icon, toggle_theme_cb},
        {"", nullptr},
        {"", nullptr},
    }};

    /* ESC maps to index 4 in shortcut mode. Enter is handled in ShortcutController. */
    const std::array<IconSpec, 5> confirm_icons = {{
        {"", nullptr},
        {"", nullptr},
        {"", nullptr},
        {"", confirm_power_action_cb},
        {"", cancel_power_action_cb},
    }};

    platform::set_nav_shortcut_mode(is_confirm_page);

    const auto& shortcuts = is_confirm_page ? kConfirmShortcuts : kHomeShortcuts;
    for (size_t index = 0; index < shortcut_key_labels_.size(); ++index) {
        auto* key_label = shortcut_key_labels_[index];
        if (!key_label || !lv_obj_is_valid(key_label)) {
            continue;
        }
        auto* item = lv_obj_get_parent(key_label);
        auto* action_label = item ? lv_obj_get_child(item, 1) : nullptr;
        const auto& shortcut = shortcuts[index];
        if (shortcut.key[0] == '\0') {
            if (item) {
                lv_obj_add_flag(item, LV_OBJ_FLAG_HIDDEN);
            }
            continue;
        }
        if (item) {
            lv_obj_remove_flag(item, LV_OBJ_FLAG_HIDDEN);
        }
        lv_label_set_text(key_label, shortcut.key);
        // Phosphor glyphs (Select arrows) need the icon face; text keys use Montserrat.
        const bool icon_key = !is_confirm_page && index == 0 && shortcut_icon_font_;
        lv_obj_set_style_text_font(key_label,
                                   icon_key ? shortcut_icon_font_ : shortcut_key_font_,
                                   0);
        lv_obj_set_style_text_color(key_label, colors.primary, 0);
        if (action_label) {
            lv_label_set_text(action_label, shortcut.action);
        }
    }

    if (shortcut_bar_) {
        lv_obj_remove_flag(shortcut_bar_, LV_OBJ_FLAG_HIDDEN);
    }

    const auto& icons = is_confirm_page ? confirm_icons : home_icons;
    for (size_t i = 0; i < icon_buttons_.size(); ++i) {
        auto& button = icon_buttons_[i];
        if (!button) {
            continue;
        }

        button->set_text(icons[i].text);
        if (button->root()) {
            // Shortcut hint bar owns the visible chrome; keep nav targets for keys only.
            lv_obj_add_flag(button->root(), LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_event_cb(button->root(), toggle_theme_cb);
            lv_obj_remove_event_cb(button->root(), cancel_power_action_cb);
            lv_obj_remove_event_cb(button->root(), confirm_power_action_cb);
            lv_obj_remove_event_cb(button->root(), request_quit_cb);
            if (icons[i].click_cb) {
                lv_obj_add_event_cb(button->root(), icons[i].click_cb, LV_EVENT_CLICKED, &view_model_);
                button->set_enabled(true);
            }
            else {
                button->set_enabled(false);
            }
        }
    }

    // Theme remains available via key 6 on home through the hidden nav target.
    if (!is_confirm_page && icon_buttons_[2] && icon_buttons_[2]->root()) {
        // Keep clickable for key routing even while visually hidden.
        lv_obj_add_flag(icon_buttons_[2]->root(), LV_OBJ_FLAG_CLICKABLE);
    }
    if (!is_confirm_page && icon_buttons_[0] && icon_buttons_[0]->root()) {
        lv_obj_add_flag(icon_buttons_[0]->root(), LV_OBJ_FLAG_CLICKABLE);
    }
}

void NavBar::toggle_theme_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->toggle_dark_mode();
    }
}

void NavBar::cancel_power_action_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->cancel_power_action();
    }
}

void NavBar::confirm_power_action_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->confirm_power_action();
    }
}

void NavBar::request_quit_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->request_quit();
    }
}

void NavBar::update_icons_cb(lv_observer_t* observer, lv_subject_t* subject) {
    LV_UNUSED(subject);

    auto* nav_bar = static_cast<NavBar*>(lv_observer_get_user_data(observer));
    if (nav_bar) {
        nav_bar->update_icon_buttons();
    }
}

}