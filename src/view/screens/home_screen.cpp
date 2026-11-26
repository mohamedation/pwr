#include "home_screen.h"

#include "asset_manager.h"
#include "bindings.h"
#include "theme.h"
#include "ui_const.h"

namespace screen {
namespace {

struct ActionTile {
    lv_obj_t* card{nullptr};
    lv_obj_t* icon{nullptr};
    lv_obj_t* caption{nullptr};
    model::PowerAction action{model::PowerAction::None};
    viewmodel::BaseViewModel* view_model{nullptr};
};

struct FocusBinding {
    ActionTile* restart{nullptr};
    ActionTile* shutdown{nullptr};
    viewmodel::BaseViewModel* view_model{nullptr};
};

void apply_tile_style(ActionTile& tile, bool focused) {
    if (!tile.card || !tile.view_model) {
        return;
    }

    const auto colors = view::palette(tile.view_model->is_dark_mode());
    const bool danger = tile.action == model::PowerAction::Shutdown;
    const lv_color_t accent = danger ? colors.error : colors.primary;

    lv_obj_set_style_bg_color(tile.card, focused ? colors.button : colors.surface, 0);
    lv_obj_set_style_border_color(tile.card, focused ? accent : colors.border, 0);
    lv_obj_set_style_border_width(tile.card, focused ? 2 : 1, 0);
    if (tile.icon) {
        lv_obj_set_style_text_color(tile.icon, focused ? accent : colors.text_disabled, 0);
    }
    if (tile.caption) {
        lv_obj_set_style_text_color(tile.caption, focused ? accent : colors.text, 0);
    }
}

void focus_observer(lv_observer_t* observer, lv_subject_t*) {
    auto* binding = static_cast<FocusBinding*>(lv_observer_get_user_data(observer));
    if (!binding || !binding->view_model || !binding->restart || !binding->shutdown) {
        return;
    }

    const auto focused = binding->view_model->focused_action();
    apply_tile_style(*binding->restart, focused == model::PowerAction::Restart);
    apply_tile_style(*binding->shutdown, focused == model::PowerAction::Shutdown);
}

void theme_focus_observer(lv_observer_t* observer, lv_subject_t* subject) {
    LV_UNUSED(subject);
    focus_observer(observer, subject);
}

void cleanup_focus_binding(lv_event_t* event) {
    auto* binding = static_cast<FocusBinding*>(lv_event_get_user_data(event));
    if (!binding) {
        return;
    }
    delete binding->restart;
    delete binding->shutdown;
    delete binding;
}

ActionTile* make_action_tile(lv_obj_t* parent,
                             viewmodel::BaseViewModel& view_model,
                             app::AssetManager& assets,
                             const lv_font_t* icon_font,
                             const char* icon,
                             const char* caption,
                             model::PowerAction action,
                             lv_event_cb_t click_cb) {
    auto* tile = new ActionTile{};
    tile->action = action;
    tile->view_model = &view_model;

    tile->card = lv_obj_create(parent);
    lv_obj_remove_style_all(tile->card);
    lv_obj_set_size(tile->card, 110, 78);
    lv_obj_set_style_radius(tile->card, 8, 0);
    lv_obj_set_style_bg_opa(tile->card, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(tile->card, 6, 0);
    lv_obj_set_flex_flow(tile->card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tile->card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(tile->card, 4, 0);
    lv_obj_add_flag(tile->card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(tile->card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(tile->card, click_cb, LV_EVENT_CLICKED, &view_model);

    tile->icon = lv_label_create(tile->card);
    lv_label_set_text(tile->icon, icon);
    lv_obj_set_style_text_font(tile->icon, icon_font ? icon_font : &lv_font_montserrat_28, 0);
    lv_obj_set_style_translate_x(tile->icon, 5, 0);  // +right / -left

    tile->caption = lv_label_create(tile->card);
    lv_label_set_text(tile->caption, caption);
    auto* caption_font = assets.load_standard_font(12);
    lv_obj_set_style_text_font(tile->caption, caption_font ? caption_font : &lv_font_montserrat_12, 0);

    return tile;
}

}
HomeScreen::HomeScreen(viewmodel::BaseViewModel& view_model, app::AssetManager& assets)
    : BaseScreen(view_model, assets) {
    init();
}

void HomeScreen::build_content(lv_obj_t* content) {
    auto* row = lv_obj_create(content);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 16, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(row);

    auto* icon_font = assets().load_font("Phosphor-Fill.ttf", 34);
    auto* restart = make_action_tile(row,
                                     view_model(),
                                     assets(),
                                     icon_font,
                                     view::ICON_ARROWS_CLOCKWISE,
                                     "Restart",
                                     model::PowerAction::Restart,
                                     restart_clicked_cb);
    auto* shutdown = make_action_tile(row,
                                      view_model(),
                                      assets(),
                                      icon_font,
                                      view::ICON_POWER,
                                      "Shut down",
                                      model::PowerAction::Shutdown,
                                      shutdown_clicked_cb);

    auto* binding = new FocusBinding{restart, shutdown, &view_model()};
    lv_obj_add_event_cb(row, cleanup_focus_binding, LV_EVENT_DELETE, binding);
    lv_subject_add_observer_obj(view_model().focused_action_subject(), focus_observer, row, binding);
    lv_subject_add_observer_obj(view_model().dark_mode_subject(), theme_focus_observer, row, binding);

    apply_tile_style(*restart, view_model().focused_action() == model::PowerAction::Restart);
    apply_tile_style(*shutdown, view_model().focused_action() == model::PowerAction::Shutdown);
}

void HomeScreen::restart_clicked_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->begin_restart();
    }
}

void HomeScreen::shutdown_clicked_cb(lv_event_t* event) {
    auto* view_model = static_cast<viewmodel::BaseViewModel*>(lv_event_get_user_data(event));
    if (view_model) {
        view_model->begin_shutdown();
    }
}

}