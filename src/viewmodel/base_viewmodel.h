#pragma once

#include "subjects.h"
#include "base_model.h"

#include "lvgl.h"

namespace viewmodel {

class BaseViewModel {
public:
    BaseViewModel();
    ~BaseViewModel();

    BaseViewModel(const BaseViewModel&) = delete;
    BaseViewModel& operator=(const BaseViewModel&) = delete;

    lv_subject_t* title_subject();
    lv_subject_t* prompt_subject();
    lv_subject_t* status_subject();
    lv_subject_t* dark_mode_subject();
    lv_subject_t* current_page_subject();
    lv_subject_t* focused_action_subject();
    lv_subject_t* quit_requested_subject();

    bool is_dark_mode() const;
    void set_dark_mode(bool enabled);
    void toggle_dark_mode();

    model::AppPage current_page() const;
    model::PowerAction focused_action() const;
    model::PowerAction pending_action() const;

    void show_home_page();
    void focus_previous();
    void focus_next();
    void set_focused_action(model::PowerAction action);
    void activate_focused();
    void begin_restart();
    void begin_shutdown();
    void cancel_power_action();
    bool confirm_power_action();
    void request_quit();

private:
    void publish_all();
    void set_status(const char* message);

    model::BaseModel model_;
    reactive::StringSubject<32> title_subject_;
    reactive::StringSubject<48> prompt_subject_;
    reactive::StringSubject<96> status_subject_;
    reactive::BoolSubject dark_mode_subject_;
    reactive::IntSubject current_page_subject_;
    reactive::IntSubject focused_action_subject_;
    reactive::BoolSubject quit_requested_subject_;
};

}