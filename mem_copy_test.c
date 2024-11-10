/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xbram.h"

#define WRITER_BRAM(A) ((volatile u32*)writer_px_config->MemBaseAddress)[A]
#define READER_BRAM(A) ((volatile u32*)reader_px_config->MemBaseAddress)[A]

XBram writer_x_bram;
XBram reader_x_bram;
XBram_Config* writer_px_config;
XBram_Config* reader_px_config;


int main()
{
    init_platform();

    xil_printf("\n\r===============================\n\r");

    writer_px_config = XBram_LookupConfig(XPAR_BRAM_0_DEVICE_ID);
    int x_status = XBram_CfgInitialize(&writer_x_bram, writer_px_config, writer_px_config->CtrlBaseAddress);
    xil_printf("\n\rBRAM_WRITER status: %d\n\r", x_status);

    reader_px_config = XBram_LookupConfig(XPAR_BRAM_1_DEVICE_ID);
    x_status = XBram_CfgInitialize(&reader_x_bram, reader_px_config, reader_px_config->CtrlBaseAddress);
    xil_printf("BRAM_READER status: %d\n\r", x_status);

    // read BRAMs
    uint32_t addr = 0xFF;
    uint32_t data_read = 0;

    while (1)
    {
		data_read = WRITER_BRAM(addr);
		xil_printf("\n\rWRITER_BRAM addr: 0x%0X: 0x%0X\n\r", addr, data_read);
		data_read = READER_BRAM(addr);
		xil_printf("READER_BRAM addr: 0x%0X: 0x%0X\n\r", addr, data_read);

		// write BRAM
		xil_printf("Writing to BRAM...\n\r");

		uint32_t data_write = (254 << 24 | 70 << 16 | 160 << 8 | 26);
		WRITER_BRAM(addr) = data_write;

		sleep(1);

		// verify write
		data_read = WRITER_BRAM(addr);
		xil_printf("WRITER_BRAM addr: 0x%0X: 0x%0X\n\r", addr, data_read);

		// verify read
		data_read = READER_BRAM(addr);
		xil_printf("READER_BRAM addr: 0x%0X: 0x%0X\n\r", addr, data_read);
    }

    cleanup_platform();
    return 0;
}
