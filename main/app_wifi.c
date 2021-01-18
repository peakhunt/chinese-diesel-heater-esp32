#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "app_wifi.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define WIFI_AP_SSID        "heater"
#define WIFI_AP_PASS        "chinesedieselheater"
#define WIFI_AP_CHANNEL     6
#define WIFI_AP_MAX_CONN    2

#define WIFI_SSID           "hkim-rnd"
#define WIFI_PASSWORD       "hkrnd1234"

static const char* TAG = "app_wifi";

//
// XXX
// this event_handler is executed in another task context!
//
static void
event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    esp_wifi_connect();
    ESP_LOGI(TAG,"connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
  }
}

static void
app_wifi_init_ap_sta(void)
{
  esp_netif_create_default_wifi_sta();
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));

  static wifi_config_t sta_wifi_config =
  {
    .sta =
    {
      .ssid     = WIFI_SSID,
      .password = WIFI_PASSWORD,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,

      .pmf_cfg = {
        .capable = true,
        .required = false
      },
    },
  };

  static wifi_config_t ap_wifi_config =
  {
    .ap = {
      .ssid = WIFI_AP_SSID,
      .ssid_len = strlen(WIFI_AP_SSID),
      .channel = WIFI_AP_CHANNEL,
      .password = WIFI_AP_PASS,
      .max_connection = WIFI_AP_MAX_CONN,
      .authmode = WIFI_AUTH_WPA_WPA2_PSK,
      .beacon_interval = 100,
    },
  };

  if (strlen(WIFI_AP_PASS) == 0) {
    ap_wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_wifi_config) );
  ESP_ERROR_CHECK(esp_wifi_start() );
}

void
app_wifi_init(void)
{
  app_wifi_init_ap_sta();
}
