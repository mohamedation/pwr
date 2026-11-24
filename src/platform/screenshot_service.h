#pragma once

#include <string>

namespace platform::screenshot {

struct CaptureResult {
    bool success{false};
    std::string path;
    std::string error;
};

CaptureResult capture_active_screen();
CaptureResult capture_active_screen_to(const std::string& path);

}