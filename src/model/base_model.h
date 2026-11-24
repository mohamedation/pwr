#pragma once

namespace model {

enum class AppPage {
    Home    = 0,
    Confirm = 1,
};

enum class PowerAction {
    None     = 0,
    Restart  = 1,
    Shutdown = 2,
};

class BaseModel {
public:
    const char* app_title() const;
    const char* prompt() const;

    bool dark_mode() const;
    void set_dark_mode(bool enabled);
    void toggle_dark_mode();

    AppPage current_page() const;
    void set_current_page(AppPage page);

    PowerAction focused_action() const;
    void set_focused_action(PowerAction action);
    void focus_previous();
    void focus_next();

    PowerAction pending_action() const;
    void set_pending_action(PowerAction action);
    void clear_pending_action();

private:
    bool dark_mode_ = true;
    AppPage current_page_ = AppPage::Home;
    PowerAction focused_action_ = PowerAction::Restart;
    PowerAction pending_action_ = PowerAction::None;
};

}