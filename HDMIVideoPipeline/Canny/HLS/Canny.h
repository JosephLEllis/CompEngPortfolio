#ifndef CANNY_H

#define CANNY_H

#include <hls_stream.h>
#include <hls_math.h>
#include <ap_axi_sdata.h>
#include <ap_fixed.h>
#include "hls_video.h"

#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3

#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

#define LINE_BUFF_WIDTH 1280
#define LINE_BUFF_HEIGHT 3

#define CHOICE_IMPULSE 1
#define CHOICE_AVERAGING 2
#define CHOICE_GAUSSIAN 3
#define CHOICE_SOBEL 4

typedef ap_axiu<24, 1, 1, 1> stream24_t;

typedef ap_axiu<31, 1, 1, 1> stream32_t;

typedef ap_axiu<8, 1, 1, 1> stream8_t;

typedef hls::stream<stream24_t> stream_t;

typedef hls::stream<stream32_t> streamVDMA_t;

typedef ap_fixed<24, 16> pixel_data;

typedef ap_int<16> pixel_data_not_fixed;

typedef ap_uint<8> raw_pixel_data;

typedef ap_uint<32> debug_idxPixel;

void Canny(stream_t &IN, stream_t &OUT, debug_idxPixel &idxPixelDebug, ap_uint<32> &processedPixelDebug, ap_uint<32> &idxColDebug, ap_uint<32> &idxRowDebug,
		   ap_uint<32> &SOFCounterDebug, bool &SOFDebug);

ap_fixed<24, 16> sumWindow(hls::Window<KERNEL_HEIGHT, KERNEL_WIDTH, pixel_data> &window, int width, int height);

ap_uint<8> rgb_to_grayscale(stream24_t in_rgb);

#endif

