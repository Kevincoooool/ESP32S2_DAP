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
#include "esp_partition.h"
static bool idf_flash;

static uint32_t _lba = 0;
static long old_millis;
static uint32_t _offset = 0;
static const char *TAG = "MSC_TASK";
const esp_partition_t *find_partition = NULL;
uint8_t *msc_disk;

#define README_CONTENTS \
	"This is tinyusb's MassStorage Class demo.\r\n\r\n\
If you find any bugs or get any questions, feel free to file an\r\n\
issue at github.com/hathach/tinyusb11"

enum
{
	DISK_BLOCK_NUM = 50, // 8KB is the smallest size that windows allow to mount
	DISK_BLOCK_SIZE = 512
};

#ifdef CFG_EXAMPLE_MSC_READONLY
const
#endif
	uint8_t _msc_disk[50][DISK_BLOCK_SIZE] =
		{
			//------------- Block0: Boot Sector -------------//
			// byte_per_sector    = DISK_BLOCK_SIZE; fat12_sector_num_16  = DISK_BLOCK_NUM;
			// sector_per_cluster = 1; reserved_sectors = 1;
			// fat_num            = 1; fat12_root_entry_num = 16;
			// sector_per_fat     = 1; sector_per_track = 1; head_num = 1; hidden_sectors = 0;
			// drive_number       = 0x80; media_type = 0xf8; extended_boot_signature = 0x29;
			// filesystem_type    = "FAT12   "; volume_serial_number = 0x1234; volume_label = "TinyUSB MSC";
			// FAT magic code at offset 510-511
			{
				0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x01, 0x00,
				0x01, 0x10, 0x00, 0x10, 0x00, 0xF8, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 'E', 'S', 'P', '3', '2',
				'S', '2', ' ', 'M', 'S', 'C', 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00,

				// Zero up to 2 last bytes of FAT magic code
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA},

			//------------- Block1: FAT12 Table -------------//
			{
				0xF8, 0xFF, 0xFF, 0xFF, 0x0F // // first 2 entries must be F8FF, third entry is cluster end of readme file
			},

			//------------- Block2: Root Directory -------------//
			{
				// first entry is volume label
				'E', 'S', 'P', '3', '2', 'S', '2', ' ', 'M', 'S', 'C', 0x08, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x65, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				// second entry is readme file
				'R', 'E', 'A', 'D', 'M', 'E', ' ', ' ', 'T', 'X', 'T', 0x20, 0x00, 0xC6, 0x52, 0x6D,
				0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 0x02, 0x00,
				sizeof(README_CONTENTS) - 1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
			},

			//------------- Block3: Readme Content -------------//
			README_CONTENTS};

void msc_task(void *params)
{
	(void)params;

	const char *data = "Test read amd write partition1122";
	uint8_t dest_data[1024] = {0};

	find_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
	ESP_LOGI("", "TYPE => %d", find_partition->type);
	ESP_LOGI("", "SUBTYPE => %02x", find_partition->subtype);
	ESP_LOGI("", "ADDRESS => %x", find_partition->address);
	ESP_LOGI("", "SIZE => %x", find_partition->size);
	if (find_partition == NULL)
	{
		printf("No partition found!\r\n");
		return -1;
	}
	msc_disk = (uint8_t *)heap_caps_calloc(1, DISK_BLOCK_SIZE * DISK_BLOCK_NUM, MALLOC_CAP_32BIT);
	if (msc_disk == NULL)
		return false;
	memcpy(msc_disk, _msc_disk, sizeof(_msc_disk));
	msc_disk[20] = (uint8_t)(find_partition->size / DISK_BLOCK_SIZE >> 8);
	msc_disk[19] = (uint8_t)(find_partition->size / DISK_BLOCK_SIZE & 0xff);
	// ESP_LOGI("TAG", "Erase custom partition\r\n");
	// if (esp_partition_erase_range(find_partition, 0, find_partition->size) != ESP_OK)
	// {
	// 	printf("Erase partition error");
	// 	return -1;
	// }

	// ESP_LOGI("TAG", "Write data to custom partition\r\n");
	// if (esp_partition_write(find_partition, 0, data, strlen(data) + 1) != ESP_OK)
	// { // incude '\0'
	// 	printf("Write partition data error");
	// 	return -1;
	// }

	ESP_LOGI("TAG", "Read data from custom partition\r\n");
	if (esp_partition_read(find_partition, 0, dest_data, 1024) != ESP_OK)
	{
		printf("Read partition data error");
		return -1;
	}

	ESP_LOGI("TAG", "Receive data: %s\r\n", (char *)dest_data);
	// RTOS forever loop
	while (1)
	{
		// ESP_LOGI(TAG,"Erase custom partition\r\n");
		// For ESP32-S2 this delay is essential to allow idle how to run and reset wdt
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

static void ticker_task(void *p)
{
	while (1)
	{
		if (xTaskGetTickCount() - old_millis > 1000)
		{

			//   esp_err_t err = esp_ota_set_boot_partition(find_partition);
			//   if(err)
			//     ESP_LOGE("", "BOOT ERR => %x [%d]", err, _offset);

			esp_restart();
		}
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
#if CFG_TUD_MSC
// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	(void)lun;

	const char vid[] = "Espressif";
	const char pid[] = "Mass Storage";
	const char rev[] = "1.0";

	memcpy(vendor_id, vid, strlen(vid));
	memcpy(product_id, pid, strlen(pid));
	memcpy(product_rev, rev, strlen(rev));
	ESP_LOGD(__func__, "");
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	(void)lun;
	ESP_LOGD(__func__, "");

	return true; // RAM disk is always ready
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
	(void)lun;
	ESP_LOGD(__func__, "");

	// if (find_partition == NULL)
	// {
	// 	*block_count = DISK_BLOCK_NUM;
	// 	*block_size = DISK_BLOCK_SIZE;
	// }
	// else
	// {
		*block_count = find_partition->size / DISK_BLOCK_SIZE;
		*block_size = DISK_BLOCK_SIZE;
	// }
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	(void)lun;
	(void)power_condition;
	ESP_LOGD(__func__, "");

	if (load_eject)
	{
		if (start)
		{
			// load disk storage
		}
		else
		{
			// unload disk storage
		}
	}

	return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
	(void)lun;
	ESP_LOGD(__func__, "");
	esp_err_t err = 0;
	if (find_partition == NULL || lba < 50) // first 50 lba is RAM disk, 50+ its ota partition
	{
		uint8_t *addr = &msc_disk[lba * 512] + offset;
		memcpy(buffer, addr, bufsize);
	}
	else
	{
		err |= esp_partition_read(find_partition, (lba * 512) + offset - 512, buffer, bufsize);
	}
	ESP_LOGD("", "LBA => %d, off => %d = %d, err = %d[%d]", lba, offset, (lba * 512) + offset, err, bufsize);

	return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
	(void)lun;
	ESP_LOGD(__func__, "");
	esp_err_t err = 0;
	(void)lun;
	if (buffer[0] == 0xe9 && !idf_flash)
	{ // we presume that we are having beginning of esp32 binary file when we see magic number at beginning of buffer
		ESP_LOGI("", "start flash");

		idf_flash = true;
		_lba = lba;
		esp_partition_erase_range(find_partition, 0x0, find_partition->size);
		old_millis = xTaskGetTickCount();
	}
	if (!idf_flash)
	{
		uint8_t *addr = &msc_disk[lba * 512] + offset;
		memcpy(addr, buffer, bufsize);
	}
	else
	{
		if (lba < _lba)
		{
			// ignore LBA that is lower than start update LBA, it is most likely FAT update
			return bufsize;
		}
		err = esp_partition_write(find_partition, _offset, buffer, bufsize);
		_offset += bufsize;
	}

	ESP_LOGD("", "LBA => %d, off => %d = %d, err = %d[%d]", lba, offset, (lba * 512) + offset, err, bufsize);
	return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
	// read10 & write10 has their own callback and MUST not be handled here
	ESP_LOGD(__func__, "");

	void const *response = NULL;
	uint16_t resplen = 0;

	// most scsi handled is input
	bool in_xfer = true;

	switch (scsi_cmd[0])
	{
	case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
		// Host is about to read/write etc ... better not to disconnect disk
		resplen = 0;
		break;

	default:
		// Set Sense = Invalid Command Operation
		tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

		// negative means error -> tinyusb could stall and/or response with failed status
		resplen = -1;
		break;
	}

	// return resplen must not larger than bufsize
	if (resplen > bufsize)
		resplen = bufsize;

	if (response && (resplen > 0))
	{
		if (in_xfer)
		{
			memcpy(buffer, response, resplen);
		}
		else
		{
			// SCSI output
		}
	}

	return resplen;
}
#endif
