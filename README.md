| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# ESP32S2 DAP

此例子需要先把tinyusb文件夹替换esp-idf/components/tinyusb下
*注意S2的USB端点fifo只有5个，0、1、2、3、4，所以将0x85分给cdc才能正常收发
#define EPNUM_MSC 0x01
#define EPNUM_VENDOR 0x03
```bash
uint8_t const desc_configuration[] = {
    // interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

#   if CFG_TUD_CDC
    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, 0x85, 8, 0x04, 0x84, TUD_OPT_HIGH_SPEED ? 512 : 64),
#   endif
#   if CFG_TUD_MSC
    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC, 0x80 | EPNUM_MSC, TUD_OPT_HIGH_SPEED ? 512 : 64), // highspeed 512
#   endif
#   if CFG_TUD_HID
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 6, HID_PROTOCOL_NONE, sizeof(desc_hid_report), 0x82, 64, 1),
#   endif
#   if CFG_TUD_VENDOR
    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 7, EPNUM_VENDOR, 0x80|EPNUM_VENDOR, TUD_OPT_HIGH_SPEED ? 512 : 64),
#   endif

};

```

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

- Any board with the ESP32-S2 chip

#### Menuconfig

If you want to set up the desctiptor using Menuconfig UI:

1. Execute in the terminal from the example's directory: `idf.py menuconfig`

2. Folow to `Component config -> TinyUSB -> Descriptor configuration` for all available configurations.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:



(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output:

```
I (328) example: USB initialization
I (327) TUSB:descriptors_control: Setting of a descriptor: 
.bDeviceClass       = 239
.bDeviceSubClass    = 2,
.bDeviceProtocol    = 1,
.bMaxPacketSize0    = 64,
.idVendor           = 0x00000483,
.idProduct          = 0x00000011,
.bcdDevice          = 0x00000210,
.iManufacturer      = 0x01,
.iProduct           = 0x02,
.iSerialNumber      = 0x03,
.bNumConfigurations = 0x01


I (357) example: USB initialization DONE
I (367) example: USB task started
```

Now open new terminal (ie putty). This will let to communicate with esp32 S2 over native USB.
