#include "base_model.h"

namespace model {

const char* BaseModel::app_title() const {
    return "PWR";
}

const char* BaseModel::prompt() const {
    switch (pending_action_) {
    case PowerAction::Restart:
        return "Restart the device?";
    case PowerAction::Shutdown:
        return "Shut down the device?";
    case PowerAction::None:
    default:
        return "Choose an action";
    }
}

bool BaseModel::dark_mode() const {
    return dark_mode_;
}

void BaseModel::set_dark_mode(bool enabled) {
    dark_mode_ = enabled;
}

void BaseModel::toggle_dark_mode() {
    dark_mode_ = !dark_mode_;
}

AppPage BaseModel::current_page() const {
    return current_page_;
}

void BaseModel::set_current_page(AppPage page) {
    current_page_ = page;
}

PowerAction BaseModel::focused_action() const {
    return focused_action_;
}

void BaseModel::set_focused_action(PowerAction action) {
    if (action == PowerAction::Restart || action == PowerAction::Shutdown) {
        focused_action_ = action;
    }
}

void BaseModel::focus_previous() {
    focused_action_ = PowerAction::Restart;
}

void BaseModel::focus_next() {
    focused_action_ = PowerAction::Shutdown;
}

PowerAction BaseModel::pending_action() const {
    return pending_action_;
}

void BaseModel::set_pending_action(PowerAction action) {
    pending_action_ = action;
}

void BaseModel::clear_pending_action() {
    pending_action_ = PowerAction::None;
}

}