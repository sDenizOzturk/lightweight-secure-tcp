#include "key.h"
#include "utils/randomgenerator.h"
#include <sstream>
#include <iomanip>

// Generates a random key
Key::Key()
{
    for (auto& element : m_key) {
        element = RandomGenerator::randomUint32();
    }
}

// Constructs a Key with an existing key
Key::Key(const std::array<uint32_t, 8>& initKey)
    : m_key(initKey)
{
}

// Converts the key to a hex string (for debugging/logging)
std::string Key::toString() const
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (auto value : m_key)
    {
        oss << std::setw(8) << value << " ";
    }
    return oss.str();
}

// Overloaded << operator for easy printing
std::ostream& operator<<(std::ostream& os, const Key& key)
{
    return os << key.toString();
}

// Equality operator
bool Key::operator==(const Key& other) const
{
    return m_key == other.m_key;
}

// Inequality operator
bool Key::operator!=(const Key& other) const
{
    return !(*this == other);
}
