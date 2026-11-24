#pragma once

#include <string>

namespace platform {

// Privileged system actions. Call only after explicit user confirmation.
bool safe_reboot(std::string& error);
bool safe_shutdown(std::string& error);

}