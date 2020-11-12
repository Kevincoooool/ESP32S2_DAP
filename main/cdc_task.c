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
#include "cdc_task.h"
static const char *TAG = "CDC_TASK";

void cdc_task(void *params)
{
	(void)params;

	// RTOS forever loop
	while (1)
	{
		#if CFG_TUD_CDC
		if (tud_cdc_connected())
		{
			
			// connected and there are data available
			if (tud_cdc_available())
			{
				uint8_t buf[64];

				// read and echo back
				uint32_t count = tud_cdc_read(buf, sizeof(buf));
				ESP_LOGI(TAG, "%s", buf);
				for (uint32_t i = 0; i < count; i++)
				{
					tud_cdc_write_char(buf[i]);

					if (buf[i] == '\r')
					{
						tud_cdc_write_str("\n > ");
					}
				}

				tud_cdc_write_flush();
			}
		}
#endif
		// For ESP32-S2 this delay is essential to allow idle how to run and reset wdt
		vTaskDelay(pdMS_TO_TICKS(16));
	}
}
#if CFG_TUD_CDC
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
	if (dtr && rts)
	{
		tud_cdc_write_str("Welcome to tinyUSB CDC example!!!\r\n");
	}
}
#endif