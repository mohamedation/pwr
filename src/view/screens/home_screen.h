#pragma once

#include "base_screen.h"

namespace screen {

class HomeScreen : public BaseScreen {
public:
    HomeScreen(viewmodel::BaseViewModel& view_model, app::AssetManager& assets);

private:
    void build_content(lv_obj_t* content) override;

    static void restart_clicked_cb(lv_event_t* event);
    static void shutdown_clicked_cb(lv_event_t* event);
};

}