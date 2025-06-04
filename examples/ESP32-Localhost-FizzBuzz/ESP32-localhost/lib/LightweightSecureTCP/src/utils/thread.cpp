#include "thread.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__EMSCRIPTEN__) || defined(__unix__) || defined(__MINGW32__)
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#elif defined(ESP_PLATFORM)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

struct ThreadData
{
    std::function<void()> func;
};

// -----------------------------------------------------------
// 1) POSIX pthread (Linux, macOS, etc.)
// -----------------------------------------------------------
#if defined(__linux__) || defined(__APPLE__) || defined(__EMSCRIPTEN__) || defined(__unix__) || defined(__MINGW32__)

void *threadRunner(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);
    data->func();
    delete data;
    return nullptr;
}

void Thread::runAsync(std::function<void()> func)
{
    pthread_t thread;
    ThreadData *data = new ThreadData{func};
    pthread_create(&thread, nullptr, threadRunner, data);
    pthread_detach(thread);
}

// -----------------------------------------------------------
// 2) Windows Thread
// -----------------------------------------------------------
#elif defined(_WIN32)

DWORD WINAPI threadRunner(LPVOID param)
{
    ThreadData *data = static_cast<ThreadData *>(param);
    data->func();
    delete data;
    return 0;
}

void Thread::runAsync(std::function<void()> func)
{
    ThreadData *data = new ThreadData{func};
    CreateThread(nullptr, 0, threadRunner, data, 0, nullptr);
}

// -----------------------------------------------------------
// 3) ESP32 FreeRTOS Task
// -----------------------------------------------------------
#elif defined(ESP_PLATFORM)

static void taskWrapper(void *param)
{
    ThreadData *data = static_cast<ThreadData *>(param);
    data->func();
    delete data;

    // Terminate the FreeRTOS task
    vTaskDelete(nullptr);
}

void Thread::runAsync(std::function<void()> func)
{
    ThreadData *data = new ThreadData{func};

    // Create a FreeRTOS task
    xTaskCreate(
        taskWrapper, // Task function
        "AsyncTask", // Task name
        4096,        // Stack size
        data,        // Parameter
        1,           // Priority
        nullptr      // Task handle (unused here)
    );
}

// -----------------------------------------------------------
// 4) Other Platforms (Not supported yet)
// -----------------------------------------------------------
#else
void Thread::runAsync(std::function<void()> /*func*/)
{
    // No implementation for other platforms
}
#endif
