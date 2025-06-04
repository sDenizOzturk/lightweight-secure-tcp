#pragma once

#include <iostream>
#include <sstream>
#include <string>

/**
 * Optional Singleton for enable/disable.
 * If you don't need it, remove it and always set `m_enabled = true`.
 */
class LWSDebugSettings {
public:
    static LWSDebugSettings& instance() {
        static LWSDebugSettings s;
        return s;
    }
    void setEnabled(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }
private:
    bool enabled;
    LWSDebugSettings() : enabled(false) {}
};

/**
 * An ephemeral DebugLine object that:
 *  - Buffers all operator<< calls
 *  - Prints on destructor if enabled
 *  - Also handles std::endl mid-statement
 */
class LWSDebugLine {
public:
    LWSDebugLine(const std::string& prefix, bool enabled)
        : m_prefix(prefix), m_enabled(enabled) {}

    // Destructor auto-prints leftover buffer + newline
    ~LWSDebugLine() {
        if (m_enabled && !m_buffer.str().empty()) {
            std::cout << m_prefix << m_buffer.str() << "\033[0m" << std::endl;
        }
    }

    // Template for any type (string, int, etc.)
    template <typename T>
    LWSDebugLine& operator<<(const T& value) {
        if (m_enabled) {
            m_buffer << value;
        }
        return *this;
    }

    // Overload for std::endl or any other manipulator
    LWSDebugLine& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (m_enabled) {
            if (manip == static_cast<std::ostream&(*)(std::ostream&)>(std::endl)) {
                if (!m_buffer.str().empty()) {
                    // Print prefix + buffer
                    std::cout << m_prefix << m_buffer.str() << "\033[0m" << std::endl;
                    m_buffer.str("");
                    m_buffer.clear();
                } else {
                    // Buffer is empty => print a blank line anyway
                    std::cout << m_prefix << "\033[0m" << std::endl;
                }
            } else {
                // Other manipulators like std::flush
                manip(std::cout);
            }
        }
        return *this;
    }

private:
    std::string m_prefix;
    bool m_enabled;
    std::ostringstream m_buffer;
};

/**
 * lwsdebug(prefix) function to create ephemeral DebugLine.
 *
 * Example usage:
 *   lwsdebug("[Server]") << "Started on port " << port << std::endl
 *   lwsdebug("[XTEA]") << "plaintext: " << plain << std::endl << "encrypted: " << enc;
 */
inline LWSDebugLine lwsdebug(const std::string& prefix = "") {
    bool enabled = LWSDebugSettings::instance().isEnabled();
    // If you want a space after prefix, do prefix + " "
    return LWSDebugLine(prefix + " ", enabled);
}

