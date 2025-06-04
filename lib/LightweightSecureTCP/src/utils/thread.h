#pragma once

#include <functional>

class Thread {
public:
    static void runAsync(std::function<void()> func);
};
