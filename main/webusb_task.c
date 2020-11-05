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

uint8_t const desc_ms_os_20[] =
    {
        // Set header: length, type, windows version, total length
        U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(0XB2),

        // Configuration subset header: length, type, configuration index, reserved, configuration total length
        U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION), 0, 0, U16_TO_U8S_LE(0XB2 - 0x0A),

        // Function Subset header: length, type, first interface, reserved, subset length
        U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION), 4, 0, U16_TO_U8S_LE(0XB2 - 0x0A - 0x08),

        // MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
        U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sub-compatible

        // MS OS 2.0 Registry property descriptor: length, type
        U16_TO_U8S_LE(0xB2 - 0x0A - 0x08 - 0x08 - 0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
        U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
        'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
        'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
        U16_TO_U8S_LE(0x0050), // wPropertyDataLength
                               //bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}”.
        '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'F', 0x00, '4', 0x00, '4', 0x00, 'D', 0x00, '9', 0x00, '-', 0x00,
        '0', 0x00, 'D', 0x00, '0', 0x00, '8', 0x00, '-', 0x00, '4', 0x00, '3', 0x00, 'F', 0x00, 'D', 0x00, '-', 0x00,
        '8', 0x00, 'B', 0x00, '3', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '2', 0x00, '7', 0x00, 'C', 0x00, 'A', 0x00,
        '8', 0x00, 'A', 0x00, 'F', 0x00, 'F', 0x00, 'F', 0x00, '9', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00};

#define URL "www.tinyusb.org/examples/webusb-serial"

const tusb_desc_webusb_url_t desc_url =
    {
        .bLength = 3 + sizeof(URL) - 1,
        .bDescriptorType = 3, // WEBUSB URL type
        .bScheme = 1,         // 0: http, 1: https
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
    if (web_serial_connected)
    {
        tud_vendor_write(buf, count);
    }

    // echo to cdc
    if (tud_cdc_connected())
    {
        for (uint32_t i = 0; i < count; i++)
        {
            tud_cdc_write_char(buf[i]);

            if (buf[i] == '\r')
                tud_cdc_write_char('\n');
        }
        tud_cdc_write_flush();
    }
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
// void cdc_task(void *p)
// {
//     while (1)
//     {
//         if (tud_cdc_connected())
//         {
//             // connected and there are data available
//             if (tud_cdc_available())
//             {
//                 uint8_t buf[64];

//                 uint32_t count = tud_cdc_read(buf, sizeof(buf));

//                 // echo back to both web serial and cdc
//                 echo_all(buf, count);
//             }
//         }
//     vTaskDelay(10);
//     }
// }

// Invoked when cdc when line state changed e.g connected/disconnected
// void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
// {
//     (void)itf;

//     // connected
//     if (dtr && rts)
//     {
//         // print initial message when connected
//         tud_cdc_write_str("\r\nTinyUSB WebUSB device example\r\n");
//     }
// }

// // Invoked when CDC interface received data from host
// void tud_cdc_rx_cb(uint8_t itf)
// {
//     (void)itf;
// }

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

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

void webusb_task(void *p)
{
    while (1)
    {
        
        // if (web_serial_connected)
        // {
        //     if (tud_vendor_available())
        //     {
                // uint8_t buf[64]={};
                // uint32_t count = tud_vendor_read(buf, sizeof(buf));
                // tud_vendor_write(buf, 64);
                // echo back to both web serial and cdc
                // echo_all(buf, count);
        //     }
        // }
        vTaskDelay(10);
    }
}