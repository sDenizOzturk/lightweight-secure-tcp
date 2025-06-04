#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"
#include <cstring>

#include <lightweightsecuretcp.h>

#include "../.env.h"

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

void initNVS()
{
    ESP_LOGI("[NVS]", "Initializing NVS");
    ESP_ERROR_CHECK(nvs_flash_init());
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("WiFi", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void connectToWiFi()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {};
    std::strncpy((char *)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    std::strncpy((char *)wifi_config.sta.password, WIFI_PASSWORD, sizeof(wifi_config.sta.password));

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    // Wait for connection (and IP)
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT,
                                           pdFALSE,
                                           pdTRUE,
                                           pdMS_TO_TICKS(10000));

    if (!(bits & WIFI_CONNECTED_BIT))
    {
        ESP_LOGE("[WIFI]", "Failed to connect to WiFi or get IP");
        abort(); // Or return;
    }
}

std::string getIPAddress()
{
    // Get IP address of ESP32
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_get_ip_info(netif, &ip_info);
    char ip_str[INET_ADDRSTRLEN];
    inet_ntoa_r(ip_info.ip.addr, ip_str, sizeof(ip_str));
    std::string localIP(ip_str);

    ESP_LOGI("[IP]", "Local IP: %s", localIP.c_str());
    return localIP;
}

std::string fizzBuzz(int number)
{
    std::string result;
    if (number % 3 == 0)
        result += "Fizz";
    if (number % 5 == 0)
        result += "Buzz";
    if (number % 3 != 0 && number % 5 != 0)
        result += std::string((number % 4) + 2, ' ') + "(:";
    return result;
}

extern "C" void app_main()
{
    initNVS();
    connectToWiFi();
    const std::string SERVER_IP = getIPAddress();
    const int SERVER_PORT = 5555;

    Key handshakeKey({0x12345678, 0x9ABCDEF0, 0x13572468, 0xACEBDF01,
                      0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00});
    LightweightSecureTCP::setHandshakeKey(handshakeKey);

    LightweightSecureServer *server = new LightweightSecureServer(SERVER_PORT);
    server->start();

    server->setOnHandshakeSuccess([&](HandshakeResult, Session &session)
                                  { session.setOnMessageReceived([&](const std::string &message)
                                                                 {
                                                                    ESP_LOGI("[SERVER]", "Received: %s", message.c_str());

                                                                    int received = std::stoi(message);
                                                                    int next = received + 1;
                                                                    std::string text = fizzBuzz(next);

                                                                    std::string reply = std::to_string(next);
                                                                    if (!text.empty()) reply += " " + text;

                                                                    ESP_LOGI("[SERVER]","Sending: %s", reply.c_str());
                                                                    vTaskDelay(pdMS_TO_TICKS(100));
                                                                    session.sendMessage(reply); }); });

    vTaskDelay(pdMS_TO_TICKS(100));

    LightweightSecureClient *client = new LightweightSecureClient(SERVER_IP, SERVER_PORT);

    client->setOnHandshakeSuccess([&](HandshakeResult)
                                  {
                                      ESP_LOGI("[CLIENT]", "Handshake completed.");

                                      client->setOnMessageReceived([&](const std::string &msg)
                                                                   {
                                                                       ESP_LOGI("[CLIENT]", "Received: %s", msg.c_str());

                                                                       int received = std::stoi(msg);
                                                                       int next = received + 1;
                                                                       std::string text = fizzBuzz(next);

                                                                       std::string reply = std::to_string(next);
                                                                       if (!text.empty())
                                                                           reply += " " + text;

                                                                       ESP_LOGI("[CLIENT]", "Sending: %s", reply.c_str());

                                                                       vTaskDelay(pdMS_TO_TICKS(100));

                                                                       client->sendMessage(reply); });

                                      client->sendMessage("1"); });

    client->connectToServer();

    vTaskDelay(portMAX_DELAY);

    client->disconnect();
    server->stop();

    delete client;
    delete server;
}
