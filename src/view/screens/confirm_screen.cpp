#include "confirm_screen.h"

#include "asset_manager.h"
#include "bindings.h"
#include "theme.h"
#include "ui_const.h"

namespace screen {
namespace {

struct ConfirmIconBinding {
    viewmodel::BaseViewModel* view_model{nullptr};
    lv_obj_t* icon{nullptr};
    lv_obj_t* ring{nullptr};
};

void apply_confirm_style(ConfirmIconBinding& binding) {
    if (!binding.view_model || !binding.icon || !binding.ring) {
        return;
    }

    const auto colors = view::palette(binding.view_model->is_dark_mode());
    const bool shutdown = binding.view_model->pending_action() == model::PowerAction::Shutdown;
    const lv_color_t accent = shutdown ? colors.error : colors.primary;

    lv_label_set_text(binding.icon,
                      shutdown ? view::ICON_POWER : view::ICON_ARROWS_CLOCKWISE);
    lv_obj_set_style_text_color(binding.icon, accent, 0);
    lv_obj_set_style_border_color(binding.ring, accent, 0);
    lv_obj_set_style_bg_color(binding.ring, colors.button, 0);
}

void confirm_style_observer(lv_observer_t* observer, lv_subject_t*) {
    auto* binding = static_cast<ConfirmIconBinding*>(lv_observer_get_user_data(observer));
    if (binding) {
        apply_confirm_style(*binding);
    }
}

void cleanup_confirm_binding(lv_event_t* event) {
    delete static_cast<ConfirmIconBinding*>(lv_event_get_user_data(event));
}

struct StatusColorBinding {
    viewmodel::BaseViewModel* view_model;
};

void status_theme_observer(lv_observer_t* observer, lv_subject_t*) {
    auto* label = lv_observer_get_target_obj(observer);
    auto* binding = static_cast<StatusColorBinding*>(lv_observer_get_user_data(observer));
    if (!label || !binding || !binding->view_model) {
        return;
    }
    const auto colors = view::palette(binding->view_model->is_dark_mode());
    lv_obj_set_style_text_color(label, colors.warning, 0);
}

void cleanup_status_binding(lv_event_t* event) {
    delete static_cast<StatusColorBinding*>(lv_event_get_user_data(event));
}

}
ConfirmScreen::ConfirmScreen(viewmodel::BaseViewModel& view_model, app::AssetManager& assets)
    : BaseScreen(view_model, assets) {
    init();
}

void ConfirmScreen::build_content(lv_obj_t* content) {
    auto* group = lv_obj_create(content);
    lv_obj_remove_style_all(group);
    lv_obj_set_size(group, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(group, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(group, 8, 0);
    lv_obj_clear_flag(group, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(group);

    auto* ring = lv_obj_create(group);
    lv_obj_remove_style_all(ring);
    lv_obj_set_size(ring, 72, 72);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(ring, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(ring, 3, 0);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_SCROLLABLE);

    auto* icon = lv_label_create(ring);
    auto* icon_font = assets().load_font("Phosphor-Fill.ttf", 40);
    lv_obj_set_style_text_font(icon, icon_font ? icon_font : &lv_font_montserrat_28, 0);
    lv_obj_center(icon);
    lv_obj_align(icon, LV_ALIGN_CENTER, 5, 1);

    auto* binding = new ConfirmIconBinding{&view_model(), icon, ring};
    lv_obj_add_event_cb(group, cleanup_confirm_binding, LV_EVENT_DELETE, binding);
    lv_subject_add_observer_obj(view_model().dark_mode_subject(), confirm_style_observer, group, binding);
    lv_subject_add_observer_obj(view_model().current_page_subject(), confirm_style_observer, group, binding);
    apply_confirm_style(*binding);

    auto* status = lv_label_create(group);
    lv_label_bind_text(status, view_model().status_subject(), nullptr);
    auto* status_font = assets().load_standard_font(11, app::StandardFontWeight::Regular);
    lv_obj_set_style_text_font(status, status_font ? status_font : &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_align(status, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(status, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(status, 260);
    auto* status_binding = new StatusColorBinding{&view_model()};
    lv_obj_add_event_cb(status, cleanup_status_binding, LV_EVENT_DELETE, status_binding);
    lv_subject_add_observer_obj(view_model().dark_mode_subject(),
                                status_theme_observer,
                                status,
                                status_binding);
    lv_obj_set_style_text_color(status, view::palette(view_model().is_dark_mode()).warning, 0);
}

}