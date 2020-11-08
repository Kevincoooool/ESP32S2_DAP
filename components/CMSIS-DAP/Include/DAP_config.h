/*
 * Copyright (C) 2019-2020 Zach Lee.
 *
 * Licensed under the MIT License, you may not use this file except in
 * compliance with the License.
 *
 * MIT License:
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ----------------------------------------------------------------------
 *
 * $Date:        1. December 2017
 * $Revision:    V0.0.0
 *
 * Project:      CMSIS-DAP Examples STM32F103
 * Title:        DAP_config.h CMSIS-DAP Configuration File for STM32F103
 *
 *---------------------------------------------------------------------------*/
#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__

//**************************************************************************************************
/**
\defgroup DAP_Config_Debug_gr CMSIS-DAP Debug Unit Information
\ingroup DAP_ConfigIO_gr
@{
Provides definitions about the hardware and configuration of the Debug Unit.

This information includes:
 - Definition of Cortex-M processor parameters used in CMSIS-DAP Debug Unit.
 - Debug Unit Identification strings (Vendor, Product, Serial Number).
 - Debug Unit communication packet size.
 - Debug Access Port supported modes and settings (JTAG/SWD and SWO).
 - Optional information about a connected Target Device (for Evaluation Boards).
*/

/* I removed RTE directory from the source code. Zach Lee */

//#include "../Include/swd_host.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "SWD_host.h"
/// Processor Clock of the Cortex-M MCU used in the Debug Unit.
/// This value is used to calculate the SWD/JTAG clock speed.
#define CPU_CLOCK 240000000U ///< Specifies the CPU Clock in Hz.

/// Number of processor cycles for I/O Port write operations.
/// This value is used to calculate the SWD/JTAG clock speed that is generated with I/O
/// Port write operations in the Debug Unit by a Cortex-M MCU. Most Cortex-M processors
/// require 2 processor cycles for a I/O Port Write operation.  If the Debug Unit uses
/// a Cortex-M0+ processor with high-speed peripheral I/O only 1 processor cycle might be
/// required.
#define IO_PORT_WRITE_CYCLES 2U ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0.

/// Indicate that Serial Wire Debug (SWD) communication mode is available at the Debug Access Port.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define DAP_SWD 1 ///< SWD Mode:  1 = available, 0 = not available.

/// Indicate that JTAG communication mode is available at the Debug Port.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define DAP_JTAG 1 ///< JTAG Mode: 1 = available, 0 = not available.

/// Configure maximum number of JTAG devices on the scan chain connected to the Debug Access Port.
/// This setting impacts the RAM requirements of the Debug Unit. Valid range is 1 .. 255.
#define DAP_JTAG_DEV_CNT 8U ///< Maximum number of JTAG devices on scan chain.

/// Default communication mode on the Debug Access Port.
/// Used for the command \ref DAP_Connect when Port Default mode is selected.
#define DAP_DEFAULT_PORT 1U ///< Default JTAG/SWJ Port Mode: 1 = SWD, 2 = JTAG.

/// Default communication speed on the Debug Access Port for SWD and JTAG mode.
/// Used to initialize the default SWD/JTAG clock frequency.
/// The command \ref DAP_SWJ_Clock can be used to overwrite this default setting.
#define DAP_DEFAULT_SWJ_CLOCK 1000000U ///< Default SWD/JTAG clock frequency in Hz.

/// Maximum Package Size for Command and Response data.
/// This configuration settings is used to optimize the communication performance with the
/// debugger and depends on the USB peripheral. Typical vales are 64 for Full-speed USB HID or WinUSB,
/// 1024 for High-speed USB HID and 512 for High-speed USB WinUSB.
#define DAP_PACKET_SIZE 64U ///< Specifies Packet Size in bytes.

/// Maximum Package Buffers for Command and Response data.
/// This configuration settings is used to optimize the communication performance with the
/// debugger and depends on the USB peripheral. For devices with limited RAM or USB buffer the
/// setting can be reduced (valid range is 1 .. 255).
#define DAP_PACKET_COUNT 1U ///< Specifies number of packets buffered.

/// Indicate that UART Serial Wire Output (SWO) trace is available.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define SWO_UART 0 ///< SWO UART:  1 = available, 0 = not available.

/// Maximum SWO UART Baudrate.
#define SWO_UART_MAX_BAUDRATE 10000000U ///< SWO UART Maximum Baudrate in Hz.

/// Indicate that Manchester Serial Wire Output (SWO) trace is available.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define SWO_MANCHESTER 0 ///< SWO Manchester:  1 = available, 0 = not available.

/// SWO Trace Buffer Size.
#define SWO_BUFFER_SIZE 8192U ///< SWO Trace Buffer Size in bytes (must be 2^n).

/// SWO Streaming Trace.
#define SWO_STREAM 0 ///< SWO Streaming Trace: 1 = available, 0 = not available.

/// Clock frequency of the Test Domain Timer. Timer value is returned with \ref TIMESTAMP_GET.
#define TIMESTAMP_CLOCK 240000000U ///< Timestamp clock in Hz (0 = timestamps not supported).

/// Debug Unit is connected to fixed Target Device.
/// The Debug Unit may be part of an evaluation board and always connected to a fixed
/// known device.  In this case a Device Vendor and Device Name string is stored which
/// may be used by the debugger or IDE to configure device parameters.
#define TARGET_DEVICE_FIXED 0 ///< Target Device: 1 = known, 0 = unknown;

#if TARGET_DEVICE_FIXED
#define TARGET_DEVICE_VENDOR "" ///< String indicating the Silicon Vendor
#define TARGET_DEVICE_NAME ""	///< String indicating the Target Device
#endif

/* I made a simple header file instead of the original of CMSIS */
// #include "cmsis_compiler.h"

/** Get Vendor ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
static inline uint8_t DAP_GetVendorString(char *str)
{
	(void)str;
	return (0U);
}

/** Get Product ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
static inline uint8_t DAP_GetProductString(char *str)
{
	(void)str;
	return (0U);
}

/** Get Serial Number string.
\param str Pointer to buffer to store the string.
\return String length.
*/
static inline uint8_t DAP_GetSerNumString(char *str)
{
	(void)str;
	return (0U);
}

///@}

/* Private defines -----------------------------------------------------------*/
// ATTENTION: DO NOT USE RTC GPIO16
#define PIN_SWDIO 9
#define PIN_SWCLK 10


#define PIN_TDO 14
#define PIN_TDI 15
#define PIN_nTRST 16 // optional
#define PIN_nRESET 17

// LED_BUILTIN
#define PIN_LED_CONNECTED 2
// LED_BUILTIN
#define PIN_LED_RUNNING 15
// Target Running LED           Not available

//**************************************************************************************************
/**
\defgroup DAP_Config_PortIO_gr CMSIS-DAP Hardware I/O Pin Access
\ingroup DAP_ConfigIO_gr
@{

Standard I/O Pins of the CMSIS-DAP Hardware Debug Port support standard JTAG mode
and Serial Wire Debug (SWD) mode. In SWD mode only 2 pins are required to implement the debug
interface of a device. The following I/O Pins are provided:

JTAG I/O Pin                 | SWD I/O Pin          | CMSIS-DAP Hardware pin mode
---------------------------- | -------------------- | ---------------------------------------------
TCK: Test Clock              | SWCLK: Clock         | Output Push/Pull
TMS: Test Mode Select        | SWDIO: Data I/O      | Output Push/Pull; Input (for receiving data)
TDI: Test Data Input         |                      | Output Push/Pull
TDO: Test Data Output        |                      | Input
nTRST: Test Reset (optional) |                      | Output Open Drain with pull-up resistor
nRESET: Device Reset         | nRESET: Device Reset | Output Open Drain with pull-up resistor


DAP Hardware I/O Pin Access Functions
-------------------------------------
The various I/O Pins are accessed by functions that implement the Read, Write, Set, or Clear to
these I/O Pins.

For the SWDIO I/O Pin there are additional functions that are called in SWD I/O mode only.
This functions are provided to achieve faster I/O that is possible with some advanced GPIO
peripherals that can independently write/read a single I/O pin without affecting any other pins
of the same I/O port. The following SWDIO I/O Pin functions are provided:
 - \ref PIN_SWDIO_OUT_ENABLE to enable the output mode from the DAP hardware.
 - \ref PIN_SWDIO_OUT_DISABLE to enable the input mode to the DAP hardware.
 - \ref PIN_SWDIO_IN to read from the SWDIO I/O pin with utmost possible speed.
 - \ref PIN_SWDIO_OUT to write to the SWDIO I/O pin with utmost possible speed.
*/

// Configure DAP I/O pins ------------------------------

//   LPC-Link-II HW uses buffers for debug port pins. Therefore it is not
//   possible to disable outputs SWCLK/TCK, TDI and they are left active.
//   Only SWDIO/TMS output can be disabled but it is also left active.
//   nRESET is configured for open drain mode.

/** Setup JTAG I/O pins: TCK, TMS, TDI, TDO, nTRST, and nRESET.
Configures the DAP Hardware I/O pins for JTAG mode:
 - TCK, TMS, TDI, nTRST, nRESET to output mode and set to high level.
 - TDO to input mode.
*/
static inline void PORT_JTAG_SETUP(void)
{
	gpio_pad_select_gpio(PIN_SWCLK);
	gpio_set_direction(PIN_SWCLK, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_SWDIO);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT_OUTPUT);

	GPIO_OUTPUT_SET(PIN_SWCLK, 1);
	GPIO_OUTPUT_SET(PIN_SWDIO, 1);

	gpio_pad_select_gpio(PIN_TDI);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_TDO);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_nRESET);
	gpio_set_direction(PIN_nRESET, GPIO_MODE_INPUT_OUTPUT);

	GPIO_OUTPUT_SET(PIN_TDI, 1);
	GPIO_OUTPUT_SET(PIN_TDO, 1);
	GPIO_OUTPUT_SET(PIN_nRESET, 1);
}

/** Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
Configures the DAP Hardware I/O pins for Serial Wire Debug (SWD) mode:
 - SWCLK, SWDIO, nRESET to output mode and set to default high level.
 - TDI, TDO, nTRST to HighZ mode (pins are unused in SWD mode).
*/
static inline void PORT_SWD_SETUP(void)
{
	ESP_LOGI("SWD_DELAY", "PORT_SWD_SETUP");
	gpio_pad_select_gpio(PIN_SWCLK);
	gpio_set_direction(PIN_SWCLK, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_SWDIO);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT_OUTPUT);

	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWCLK));
	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWDIO));

	gpio_pad_select_gpio(PIN_TDI);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_TDO);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT_OUTPUT);
	gpio_pad_select_gpio(PIN_nRESET);
	gpio_set_direction(PIN_nRESET, GPIO_MODE_INPUT_OUTPUT);

	GPIO_OUTPUT_SET(PIN_TDI, 1);
	GPIO_OUTPUT_SET(PIN_TDO, 1);
	GPIO_OUTPUT_SET(PIN_nRESET, 1);
}

/** Disable JTAG/SWD I/O Pins.
Disables the DAP Hardware I/O pins which configures:
 - TCK/SWCLK, TMS/SWDIO, TDI, TDO, nTRST, nRESET to High-Z mode.
*/
static inline void PORT_OFF(void)
{
	gpio_pad_select_gpio(PIN_SWCLK);
	gpio_set_direction(PIN_SWCLK, GPIO_MODE_INPUT);
	gpio_pad_select_gpio(PIN_SWDIO);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT);

	gpio_pad_select_gpio(PIN_TDI);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT);
	gpio_pad_select_gpio(PIN_TDO);
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT);
	gpio_pad_select_gpio(PIN_nRESET);
	gpio_set_direction(PIN_nRESET, GPIO_MODE_INPUT);
}

// SWCLK/TCK I/O pin -------------------------------------

/** SWCLK/TCK I/O pin: Get Input.
\return Current status of the SWCLK/TCK DAP hardware I/O pin.
*/
static inline uint32_t PIN_SWCLK_TCK_IN(void)
{
	// return (uint32_t)READ_PERI_REG(GPIO_PIN10_REG)? 1 : 0;
	return (uint32_t)GPIO_INPUT_GET(PIN_SWCLK) ? 1 : 0; 
}

/** SWCLK/TCK I/O pin: Set Output to High.
Set the SWCLK/TCK DAP hardware I/O pin to high level.
*/
static inline void PIN_SWCLK_TCK_SET(void)
{
	//GPIO_OUTPUT_SET(PIN_SWCLK, 1);
	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWCLK));
}

/** SWCLK/TCK I/O pin: Set Output to Low.
Set the SWCLK/TCK DAP hardware I/O pin to low level.
*/
static inline void PIN_SWCLK_TCK_CLR(void)
{
	// GPIO_OUTPUT_SET(PIN_SWCLK, 0);
	WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (0x1 << PIN_SWCLK));
}

// SWDIO/TMS Pin I/O --------------------------------------

/** SWDIO/TMS I/O pin: Get Input.
\return Current status of the SWDIO/TMS DAP hardware I/O pin.
*/
static inline uint32_t PIN_SWDIO_TMS_IN(void)
{
	return (uint32_t)GPIO_INPUT_GET(PIN_SWDIO) ? 1 : 0;
}

/** SWDIO/TMS I/O pin: Set Output to High.
Set the SWDIO/TMS DAP hardware I/O pin to high level.
*/
static inline void PIN_SWDIO_TMS_SET(void)
{
	// GPIO_OUTPUT_SET(PIN_SWDIO, 1);
	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWDIO));
}

/** SWDIO/TMS I/O pin: Set Output to Low.
Set the SWDIO/TMS DAP hardware I/O pin to low level.
*/
static inline void PIN_SWDIO_TMS_CLR(void)
{

	WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (0x1 << PIN_SWDIO));
}

/** SWDIO I/O pin: Get Input (used in SWD mode only).
\return Current status of the SWDIO DAP hardware I/O pin.
*/

static inline uint32_t PIN_SWDIO_IN(void)
{
	// return (uint32_t)READ_PERI_REG(GPIO_FUNC9_IN_SEL_CFG_REG)? 1 : 0;
	return (uint32_t)GPIO_INPUT_GET(PIN_SWDIO) ? 1 : 0;
}

/** SWDIO I/O pin: Set Output (used in SWD mode only).
\param bit Output value for the SWDIO DAP hardware I/O pin.
*/
static inline void PIN_SWDIO_OUT(uint32_t bit)
{
	/**
    * Important: Use only one bit (bit0) of param!
	* Sometimes the func "SWD_TransferFunction" of SW_DP.c will
	* issue "2" as param instead of "0". Zach Lee
	*/
	if ((bit & 1U) == 1)
	{
		WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWDIO));

	}
	else
	{
		WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (0x1 << PIN_SWDIO));

	}
}

/** SWDIO I/O pin: Switch to Output mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to output mode. This function is
called prior \ref PIN_SWDIO_OUT function calls.
*/
static inline void PIN_SWDIO_OUT_ENABLE(void)
{

	gpio_pad_select_gpio(PIN_SWDIO);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT_OUTPUT);
	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWDIO));
}

/** SWDIO I/O pin: Switch to Input mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to input mode. This function is
called prior \ref PIN_SWDIO_IN function calls.
*/
static inline void PIN_SWDIO_OUT_DISABLE(void)
{

	gpio_pad_select_gpio(PIN_SWDIO);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT);

	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_SWDIO));
}

// TDI Pin I/O ---------------------------------------------

/** TDI I/O pin: Get Input.
\return Current status of the TDI DAP hardware I/O pin.
*/
static inline uint32_t PIN_TDI_IN(void)
{
	// return (uint32_t)READ_PERI_REG(GPIO_IN_REG&(0x1 << PIN_TDI))? 1 : 0;
	return (uint32_t)GPIO_INPUT_GET(PIN_TDI) ? 1 : 0;
}

/** TDI I/O pin: Set Output.
\param bit Output value for the TDI DAP hardware I/O pin.
*/
static inline void PIN_TDI_OUT(uint32_t bit)
{

	if ((bit & 1U) == 1)
	{
		WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (0x1 << PIN_TDI));
		// GPIO_OUTPUT_SET(PIN_TDI, 1);
		// JTAG_TDI_GPIO_Port->BSRR = JTAG_TDI_Pin;
	}
	else
	{
		WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (0x1 << PIN_TDI));
		// GPIO_OUTPUT_SET(PIN_TDI, 0);
		// JTAG_TDI_GPIO_Port->BRR = JTAG_TDI_Pin;
	}
}

// TDO Pin I/O ---------------------------------------------

/** TDO I/O pin: Get Input.
\return Current status of the TDO DAP hardware I/O pin.
*/
static inline uint32_t PIN_TDO_IN(void)
{
	return (uint32_t)GPIO_INPUT_GET(PIN_TDO) ? 1 : 0;
	// return (uint32_t)READ_PERI_REG(GPIO_IN_REG&(0x1 << PIN_TDO))? 1 : 0;
	//   return (uint32_t)(JTAG_TDO_GPIO_Port->IDR & JTAG_TDO_Pin ? 1:0);
}

// nTRST Pin I/O -------------------------------------------

/** nTRST I/O pin: Get Input.
\return Current status of the nTRST DAP hardware I/O pin.
*/
static inline uint32_t PIN_nTRST_IN(void)
{
	return (0U); // Not available
}

/** nTRST I/O pin: Set Output.
\param bit JTAG TRST Test Reset pin status:
           - 0: issue a JTAG TRST Test Reset.
           - 1: release JTAG TRST Test Reset.
*/
static inline void PIN_nTRST_OUT(uint32_t bit)
{
	(void)bit;
	; // Not available
}

// nRESET Pin I/O------------------------------------------

/** nRESET I/O pin: Get Input.
\return Current status of the nRESET DAP hardware I/O pin.
*/
static inline uint32_t PIN_nRESET_IN(void)
{
	return (0U); //
				 //   return (uint32_t)(JTAG_nRESET_GPIO_Port->ODR & JTAG_nRESET_Pin ? 1:0);
}

/** nRESET I/O pin: Set Output.
\param bit target device hardware reset pin status:
           - 0: issue a device hardware reset.
           - 1: release device hardware reset.
*/
static inline void PIN_nRESET_OUT(uint32_t bit)
{

	  if ((bit & 1U) == 1) {

	  } else {

		if(swd_init_debug())
		{
			ESP_LOGI("RST","Connect");
		}
		else
		{
			ESP_LOGI("RST","Disconnect");
		}

		// uint32_t swd_mem_write_data = 0x05FA0000 | 0x4;
		// swd_write_memory(0xE000ED0C,(uint8_t*)&swd_mem_write_data,4);
	  }
}

///@}

//**************************************************************************************************
/**
\defgroup DAP_Config_LEDs_gr CMSIS-DAP Hardware Status LEDs
\ingroup DAP_ConfigIO_gr
@{

CMSIS-DAP Hardware may provide LEDs that indicate the status of the CMSIS-DAP Debug Unit.

It is recommended to provide the following LEDs for status indication:
 - Connect LED: is active when the DAP hardware is connected to a debugger.
 - Running LED: is active when the debugger has put the target device into running state.
*/

/** Debug Unit: Set status of Connected LED.
\param bit status of the Connect LED.
           - 1: Connect LED ON: debugger is connected to CMSIS-DAP Debug Unit.
           - 0: Connect LED OFF: debugger is not connected to CMSIS-DAP Debug Unit.
*/
static inline void LED_CONNECTED_OUT(uint32_t bit)
{

	if ((bit & 1U) == 1)
	{
		gpio_set_level(PIN_LED_CONNECTED, 1);
		// LED_GPIO_Port->BRR =  LED_CONNECTED_Pin;
	}
	else
	{
		gpio_set_level(PIN_LED_CONNECTED, 0);
		// LED_GPIO_Port->BSRR = LED_CONNECTED_Pin;
	}
}

/** Debug Unit: Set status Target Running LED.
\param bit status of the Target Running LED.
           - 1: Target Running LED ON: program execution in target started.
           - 0: Target Running LED OFF: program execution in target stopped.
*/
static inline void LED_RUNNING_OUT(uint32_t bit)
{
	(void)bit;
	; // Not available
}

///@}

//**************************************************************************************************
/**
\defgroup DAP_Config_Timestamp_gr CMSIS-DAP Timestamp
\ingroup DAP_ConfigIO_gr
@{
Access function for Test Domain Timer.

The value of the Test Domain Timer in the Debug Unit is returned by the function \ref TIMESTAMP_GET. By
default, the DWT timer is used.  The frequency of this timer is configured with \ref TIMESTAMP_CLOCK.

*/

/** Get timestamp of Test Domain Timer.
\return Current timestamp value.
*/
// static inline uint32_t TIMESTAMP_GET (void) {
//   return (DWT->CYCCNT);
// }
static inline uint32_t TIMESTAMP_GET(void)
{
	return xTaskGetTickCount();
}
///@}

//**************************************************************************************************
/**
\defgroup DAP_Config_Initialization_gr CMSIS-DAP Initialization
\ingroup DAP_ConfigIO_gr
@{

CMSIS-DAP Hardware I/O and LED Pins are initialized with the function \ref DAP_SETUP.
*/

/** Setup of the Debug Unit I/O pins and LEDs (called when Debug Unit is initialized).
This function performs the initialization of the CMSIS-DAP Hardware I/O Pins and the
Status LEDs. In detail the operation of Hardware I/O and LED pins are enabled and set:
 - I/O clock system enabled.
 - all I/O pins: input buffer enabled, output pins are set to HighZ mode.
 - for nTRST, nRESET a weak pull-up (if available) is enabled.
 - LED output pins are enabled and LEDs are turned off.
*/
static inline void DAP_SETUP(void)
{

	PORT_JTAG_SETUP();
	PORT_SWD_SETUP();
	gpio_set_direction(PIN_SWCLK, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_direction(PIN_SWDIO, GPIO_MODE_INPUT_OUTPUT);	//
	gpio_set_direction(PIN_nRESET, GPIO_MODE_INPUT_OUTPUT); //
	gpio_set_direction(PIN_TDI, GPIO_MODE_INPUT_OUTPUT);
	gpio_set_direction(PIN_TDO, GPIO_MODE_INPUT_OUTPUT);
	// Configure: LED as output (turned off)
	gpio_set_direction(PIN_LED_CONNECTED, GPIO_MODE_DEF_OUTPUT);
	LED_CONNECTED_OUT(0);
	gpio_set_direction(PIN_LED_RUNNING, GPIO_MODE_DEF_OUTPUT);
	LED_RUNNING_OUT(0);
}

/** Reset Target Device with custom specific I/O pin or command sequence.
This function allows the optional implementation of a device specific reset sequence.
It is called when the command \ref DAP_ResetTarget and is for example required
when a device needs a time-critical unlock sequence that enables the debug port.
\return 0 = no device specific reset sequence is implemented.\n
        1 = a device specific reset sequence is implemented.
*/
static inline uint8_t RESET_TARGET(void)
{
	return (0U); // change to '1' when a device reset sequence is implemented
}

///@}

#endif /* __DAP_CONFIG_H__ */
