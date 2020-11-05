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
#include "../components/CMSIS-DAP/Include/DAP.h"

static const char *TAG = "HID_TASK";

uint8_t MYUSB_Request[64 + 1];	 // Request  Buffer
uint8_t MYUSB_Response[64 + 1]; // Response Buffer
uint8_t dealing_data = 0;
int hid_len;
void hid_task(void *params)
{
	(void)params;

	// RTOS forever loop
	while (1)
	{
		if (dealing_data)
		{
			DAP_ProcessCommand(MYUSB_Request, MYUSB_Response);
			tud_hid_report(0, MYUSB_Response, 64);
			dealing_data = 0;
		}
		// For ESP32-S2 this delay is essential to allow idle how to run and reset wdt
		vTaskDelay(pdMS_TO_TICKS(5));
		// vTaskDelay(10 / portTICK_RATE_MS);
	}
	//vTaskDelay(pdMS_TO_TICKS(5));
}
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
	// TODO not Implemented
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;

	return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
	// This example doesn't use multiple report and report ID
	(void)report_id;
	(void)report_type;
	// ESP_LOGI(TAG, "%s", buffer);
	if (buffer[0] == ID_DAP_TransferAbort)
	{
		DAP_TransferAbort = 1;
		return;
	}
	//没有在处理数据过程中才会接收 不然直接退出
	if (dealing_data)
		return; // Discard packet when buffer is full
	memcpy(MYUSB_Request, buffer, bufsize);
	dealing_data = 1;
	
	// echo back anything we received from host
	// tud_hid_report(0, buffer, bufsize);
}
