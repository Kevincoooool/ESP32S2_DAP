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
#include "DAP_config.h"
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
		// GPIO_OUTPUT_SET(6,0);
		// ESP_LOGI(TAG, "IO:%d",GPIO_INPUT_GET(3));
		// vTaskDelay(pdMS_TO_TICKS(500));
		// GPIO_OUTPUT_SET(6,1);
		// ESP_LOGI(TAG, "IO:%d",GPIO_INPUT_GET(3));
		// vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void app_main(void)
{

	gpio_pad_select_gpio(3);
	gpio_set_direction(3, GPIO_MODE_INPUT);

	gpio_pad_select_gpio(6);
	gpio_set_direction(6, GPIO_MODE_INPUT_OUTPUT);

	ESP_LOGI(TAG, "USB initialization");

	tinyusb_config_t tusb_cfg = {0};

	ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
	ESP_LOGI(TAG, "USB initialization DONE");

	// Create a task for tinyusb device stack:
	xTaskCreate(usb_device_task, "usbd", 4096, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(hid_task, "msc", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
	xTaskCreate(cdc_task, "cdc", 4096, NULL, 8, NULL);
	xTaskCreate(msc_task, "msc", 4096, NULL, 8, NULL);

	return;
}
