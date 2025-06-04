#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <ostream>

class Key
{
public:
    Key();  // Generates a random key
    explicit Key(const std::array<uint32_t, 8>& initKey);

    std::string toString() const;

    bool operator==(const Key& other) const;
    bool operator!=(const Key& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Key& key); // Overloaded << operator

    operator std::array<uint32_t, 8>() const { return m_key; }

private:
    std::array<uint32_t, 8> m_key;
};
