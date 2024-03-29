/*
 * @Descripttion : 
 * @version      : 
 * @Author       : Kevincoooool
 * @Date         : 2020-11-03 21:06:24
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2020-11-12 20:43:52
 * @FilePath     : \ESP32S2_DAP\main\main.c
 */
/* USB Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "tinyusb.h"
#include "msc_task.h"
#include "cdc_task.h"
#include "hid_task.h"
#include "webusb_task.h"
#include "DAP_config.h"
#include "DAP.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "my_tcp.h"
static const char *TAG = "example";

// USB Device Driver task
// This top level thread processes all usb events and invokes callbacks
static void usb_device_task(void *param)
{
	(void)param;
	ESP_LOGI(TAG, "USB task started");
	while (1)
	{
		tud_task(); // RTOS forever loop
	}
}
static void led_task(void *param)//
{
	(void)param;
	ESP_LOGI(TAG, "LED task started");
	while (1)
	{

		WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << 9));
		WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (0x1 << 9));
	}
}
void app_main(void)
{

	gpio_pad_select_gpio(3);
	gpio_set_direction(3, GPIO_MODE_INPUT);

	gpio_pad_select_gpio(9);
	gpio_set_direction(9, GPIO_MODE_INPUT_OUTPUT);

	DAP_SETUP();
	ESP_LOGI(TAG, "USB initialization");

	tinyusb_config_t tusb_cfg = {0};

	ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
	ESP_LOGI(TAG, "USB initialization DONE");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// #if TCP_SERVER_CLIENT_OPTION
	//     ESP_LOGI(TAG, "As a Tcp Server , will start wifi_init_softap...");
	//     wifi_init_softap();
	// #else

	//     ESP_LOGI(TAG, "As a Tcp Client , will start wifi_init_sta...");
	//     wifi_init_sta();
	// #endif
	//     xTaskCreate(&tcp_conn, "tcp_conn", 4096, NULL, 8, NULL);

	// Create a task for tinyusb device stack:
	xTaskCreate(usb_device_task, "usbd", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(hid_task, "hid", 4096, NULL, configMAX_PRIORITIES, NULL);
	xTaskCreate(cdc_task, "cdc", 4096, NULL, 8, NULL);
	xTaskCreate(msc_task, "msc", 4096, NULL, 8, NULL);
	xTaskCreate(webusb_task, "web", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
	//	xTaskCreate(led_task, "led", 4096, NULL, 20, NULL);

	return;
}
