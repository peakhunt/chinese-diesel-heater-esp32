#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "app_wifi.h"
#include "app_heater.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif

void app_main(void)
{
  printf("Diesel Heater!\n");

  esp_err_t ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
      CHIP_NAME,
      chip_info.cores,
      (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
      (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);

  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
      (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

  app_heater_init();
  app_wifi_init();

  while(1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
#if 0
  for(int i = 10; i >= 0; i--)
  {
    printf("Restarting in %d seconds...\n", i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  printf("Restarting now.\n");
  fflush(stdout);

  esp_restart();
#endif
}
