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
#include "webusb_task.h"
#include "DAP.h"

static const char *TAG = "WEB_TASK";

enum
{
	VENDOR_REQUEST_WEBUSB = 1,
	VENDOR_REQUEST_MICROSOFT = 2
};

#define URL "CMSIS-DAP v2"
#if CFG_TUD_VENDOR
const tusb_desc_webusb_url_t desc_url =
	{
		.bLength = 3 + sizeof(URL) - 1,
		.bDescriptorType = 3, // WEBUSB URL type
		.bScheme = 1,		  // 0: http, 1: https
		.url = URL};

static bool web_serial_connected = false;

//------------- prototypes -------------//
void cdc_task(void *);
void webusb_task(void *p);

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
	ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
	ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
	ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
	ESP_LOGI(TAG, "%s", __func__);
}

// send characters to both CDC and WebUSB
static void echo_all(uint8_t buf[], uint32_t count)
{
	// echo to web serial
	if (tud_vendor_write_available())
	{
		tud_vendor_write(buf, count);
	}
	else
		ESP_LOGI(TAG, "%s", __func__);

	// // echo to cdc
	// if (tud_cdc_connected())
	// {
	// 	for (uint32_t i = 0; i < count; i++)
	// 	{
	// 		tud_cdc_write_char(buf[i]);

	// 		if (buf[i] == '\r')
	// 			tud_cdc_write_char('\n');
	// 	}
	// 	tud_cdc_write_flush();
	// }
}

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+
extern uint8_t const desc_ms_os_20[];
// Invoked when received VENDOR control request
bool tud_vendor_control_request_cb(uint8_t rhport, tusb_control_request_t const *request)
{
	ESP_LOGI(TAG, "%s", __func__);
	switch (request->bRequest)
	{
	case VENDOR_REQUEST_WEBUSB:
		// match vendor request in BOS descriptor
		// Get landing page url
		return tud_control_xfer(rhport, request, (void *)&desc_url, desc_url.bLength);

	case VENDOR_REQUEST_MICROSOFT:
		if (request->wIndex == 7)
		{
			// Get Microsoft OS 2.0 compatible descriptor
			uint16_t total_len;
			memcpy(&total_len, desc_ms_os_20 + 8, 2);

			return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, total_len);
		}
		else
		{
			return false;
		}

	case 0x22:
		// Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
		// connect and disconnect.
		web_serial_connected = (request->wValue != 0);

		// Always lit LED if connected
		if (web_serial_connected)
		{
			tud_vendor_write_str("\r\nTinyUSB WebUSB device example\r\n");
		}

		// response with status OK
		return tud_control_status(rhport, request);

	default:
		// stall unknown request

		// return tud_control_xfer(rhport, request, (void *)&desc_url, desc_url.bLength);
		return false;
	}

	return true;
}

// Invoked when DATA Stage of VENDOR's request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const *request)
{
	(void)rhport;
	(void)request;

	// nothing to do
	return true;
}
uint8_t WINUSB_Request[64] = {0};  // Request  Buffer
uint8_t WINUSB_Response[64] = {0}; // Response Buffer
uint8_t WINUSB_data = 0;
int WINUSB_len;
void webusb_task(void *p)
{
	uint32_t time;
	while (1)
	{
		// if (web_serial_connected)
		// {
		if (tud_vendor_available())
		{

			time = xTaskGetTickCount();

			uint32_t count = tud_vendor_read(WINUSB_Request, 64);
			DAP_ProcessCommand(WINUSB_Request, WINUSB_Response);
			tud_vendor_write(WINUSB_Response, 63);
			ESP_LOGI(TAG, "%d %d", count, xTaskGetTickCount() - time);
		}
		vTaskDelay(pdMS_TO_TICKS(10));
		// }
		// vTaskDelay(pdMS_TO_TICKS(2));
	}
}
