#pragma once

#include "base_screen.h"

namespace screen {

class ConfirmScreen : public BaseScreen {
public:
    ConfirmScreen(viewmodel::BaseViewModel& view_model, app::AssetManager& assets);

private:
    void build_content(lv_obj_t* content) override;
};

}