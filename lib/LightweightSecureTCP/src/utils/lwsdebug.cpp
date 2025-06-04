#include "lwsdebug.h"

LWSDebugLine lwsdebug(const std::string& prefix) {
    bool enabled = LWSDebugSettings::instance().isEnabled();
    return LWSDebugLine(prefix + " ", enabled);
}
