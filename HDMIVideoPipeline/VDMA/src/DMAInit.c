#include <stdio.h>
#include "platform.h"
#include "xil_io.h"
#include "xil_printf.h"

#define HLINE 3840 // (1280*3 bytes)
#define VLINE 720

void configureWriteVDMA(){

	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);

	// Set the start address of the first frame buffer, located at 0x100000.
	// Frame buffer is 3 bytes * 720 * 1280 = 2764800 bytes, so each address will be offset by 2764800 (0x2A3000) bytes.
	// Second frame buffer needs to be 0x3A3000 or higher.

	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, 0x100000); // Frame buffer 1 start addr.
	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB0, 0x3A3000); // Frame buffer 2 start addr.
	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xB4, 0x646000); // Frame buffer 3 start addr.

	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, 1280*3); // STRIDE (number of bytes to skip to get to the next line) is equal to HLINE (# of bytes per line)
	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, 1280*3); // 1280*3 bytes per line

	Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, VLINE);

	return;
};

void configureReadVDMA(){

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C, 0x100000);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x60, 0x3A3000);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x64, 0x646000);

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, 1280*3);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, 1280*3);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, VLINE);

    return;
};


int main()
{
    init_platform();

    configureReadVDMA();
    configureWriteVDMA();

    cleanup_platform();
    return 0;
}
