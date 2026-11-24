#include "base_viewmodel.h"

#include "power_control.h"

#include <string>

namespace viewmodel {
namespace {

int page_to_int(model::AppPage page) {
    return static_cast<int>(page);
}

int action_to_int(model::PowerAction action) {
    return static_cast<int>(action);
}

}
BaseViewModel::BaseViewModel()
    : title_subject_(model_.app_title()),
      prompt_subject_(model_.prompt()),
      status_subject_(""),
      dark_mode_subject_(model_.dark_mode()),
      current_page_subject_(page_to_int(model_.current_page())),
      focused_action_subject_(action_to_int(model_.focused_action())),
      quit_requested_subject_(false) {}

BaseViewModel::~BaseViewModel() = default;

lv_subject_t* BaseViewModel::title_subject() {
    return title_subject_.native();
}

lv_subject_t* BaseViewModel::prompt_subject() {
    return prompt_subject_.native();
}

lv_subject_t* BaseViewModel::status_subject() {
    return status_subject_.native();
}

lv_subject_t* BaseViewModel::dark_mode_subject() {
    return dark_mode_subject_.native();
}

lv_subject_t* BaseViewModel::current_page_subject() {
    return current_page_subject_.native();
}

lv_subject_t* BaseViewModel::focused_action_subject() {
    return focused_action_subject_.native();
}

lv_subject_t* BaseViewModel::quit_requested_subject() {
    return quit_requested_subject_.native();
}

bool BaseViewModel::is_dark_mode() const {
    return model_.dark_mode();
}

void BaseViewModel::set_dark_mode(bool enabled) {
    model_.set_dark_mode(enabled);
    publish_all();
}

void BaseViewModel::toggle_dark_mode() {
    model_.toggle_dark_mode();
    publish_all();
}

model::AppPage BaseViewModel::current_page() const {
    return model_.current_page();
}

model::PowerAction BaseViewModel::focused_action() const {
    return model_.focused_action();
}

model::PowerAction BaseViewModel::pending_action() const {
    return model_.pending_action();
}

void BaseViewModel::show_home_page() {
    model_.clear_pending_action();
    model_.set_current_page(model::AppPage::Home);
    set_status("");
    publish_all();
}

void BaseViewModel::focus_previous() {
    model_.focus_previous();
    publish_all();
}

void BaseViewModel::focus_next() {
    model_.focus_next();
    publish_all();
}

void BaseViewModel::set_focused_action(model::PowerAction action) {
    model_.set_focused_action(action);
    publish_all();
}

void BaseViewModel::activate_focused() {
    switch (model_.focused_action()) {
    case model::PowerAction::Shutdown:
        begin_shutdown();
        break;
    case model::PowerAction::Restart:
    default:
        begin_restart();
        break;
    }
}

void BaseViewModel::begin_restart() {
    model_.set_focused_action(model::PowerAction::Restart);
    model_.set_pending_action(model::PowerAction::Restart);
    model_.set_current_page(model::AppPage::Confirm);
    set_status("");
    publish_all();
}

void BaseViewModel::begin_shutdown() {
    model_.set_focused_action(model::PowerAction::Shutdown);
    model_.set_pending_action(model::PowerAction::Shutdown);
    model_.set_current_page(model::AppPage::Confirm);
    set_status("");
    publish_all();
}

void BaseViewModel::cancel_power_action() {
    show_home_page();
}

bool BaseViewModel::confirm_power_action() {
    std::string error;
    bool ok = false;

    switch (model_.pending_action()) {
    case model::PowerAction::Restart:
        set_status("Restarting...");
        ok = platform::safe_reboot(error);
        break;
    case model::PowerAction::Shutdown:
        set_status("Shutting down...");
        ok = platform::safe_shutdown(error);
        break;
    case model::PowerAction::None:
    default:
        set_status("No action selected");
        return false;
    }

    if (!ok) {
        set_status(error.c_str());
    }
    return ok;
}

void BaseViewModel::request_quit() {
    quit_requested_subject_.set(true);
}

void BaseViewModel::publish_all() {
    title_subject_.set(model_.app_title());
    prompt_subject_.set(model_.prompt());
    dark_mode_subject_.set(model_.dark_mode());
    current_page_subject_.set(page_to_int(model_.current_page()));
    focused_action_subject_.set(action_to_int(model_.focused_action()));
}

void BaseViewModel::set_status(const char* message) {
    status_subject_.set(message ? message : "");
}

}