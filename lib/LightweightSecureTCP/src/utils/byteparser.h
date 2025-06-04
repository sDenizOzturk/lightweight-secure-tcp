#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
class ByteParser
{
public:
    template<typename T>
    static void appendToPacket(std::vector<uint8_t> &payload, const T &value) {
        payload.insert(payload.end(), reinterpret_cast<const uint8_t*>(&value),
                       reinterpret_cast<const uint8_t*>(&value) + sizeof(value));
    }

    static void appendToPacket(std::vector<uint8_t>& payload, const std::vector<uint8_t>& value) {
        payload.insert(payload.end(), value.begin(), value.end());
    }

    template <typename T>
    static bool extractFromPacket(const std::vector<uint8_t>& payload, T& value, size_t& offset) {
        if (offset + sizeof(T) > payload.size()) {
            return false; // Offset out of range
        }
        std::memcpy(&value, payload.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    template <typename T>
    static bool extractFromPacket(const std::vector<uint8_t>& payload, T& value) {
        size_t offset = 0;
        return extractFromPacket(payload, offset, value);
    }

    static bool extractFromPacket(const std::vector<uint8_t>& payload, std::vector<uint8_t>& value, size_t& offset, size_t length) {
        if (offset + length > payload.size()) {
            return false; // Offset out of range
        }

        value.assign(payload.begin() + offset, payload.begin() + offset + length);
        offset += length;
        return true;
    }

    template <typename T, std::size_t N>
    static bool extractFromPacket(const std::vector<uint8_t>& packet, std::array<T, N>& value, size_t& offset) {
        if (offset + sizeof(T) * N > packet.size()) {
            return false; // Offset out of range
        }
        std::memcpy(value.data(), packet.data() + offset, sizeof(T) * N);
        offset += sizeof(T) * N;
        return true;
    }

    template <typename T, std::size_t N>
    static bool extractFromPacket(const std::vector<uint8_t>& packet, std::array<T, N>& value) {
        size_t offset = 0;
        return extractFromPacket(packet, value, offset);
    }
};

