#include "power_control.h"

#include <cstdlib>

namespace platform {
namespace {

bool run_power_command(const char* primary, const char* fallback, std::string& error) {
#if USE_DESKTOP
    (void)primary;
    (void)fallback;
    error = "power actions are unavailable in the desktop simulator";
    return false;
#else
    if (std::system(primary) == 0) {
        return true;
    }
    if (fallback && std::system(fallback) == 0) {
        return true;
    }
    error = "failed to run privileged power command (check polkit/sudo rights)";
    return false;
#endif
}

}
bool safe_reboot(std::string& error) {
    error.clear();
    return run_power_command("systemctl reboot", "reboot", error);
}

bool safe_shutdown(std::string& error) {
    error.clear();
    return run_power_command("systemctl poweroff", "poweroff", error);
}

}