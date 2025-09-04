#include "Canny.h"
#include <iostream>

#define DEBUG_MODE 0

#ifdef DEBUG_MODE

#undef FRAME_WIDTH
#undef FRAME_HEIGHT
#define FRAME_WIDTH 256
#define FRAME_HEIGHT 144

#endif


// Function to convert line of RGB pixel data to grayscale format:
ap_uint<8> rgb_to_grayscale(stream24_t in_rgb){
#pragma HLS inline

     ap_ufixed<16, 8> red = in_rgb.data.range(23, 16);
     ap_ufixed<16, 8> blue = in_rgb.data.range(15, 8);
     ap_ufixed<16, 8> green = in_rgb.data.range(7, 0);

     ap_fixed<16, 8> RED_GRAYSCALE_CONST = 0.299;
     ap_fixed<16, 8> BLUE_GRAYSCALE_CONST = 0.114;
     ap_fixed<16, 8>  GREEN_GRAYSCALE_CONST = 0.587;

     red = red * RED_GRAYSCALE_CONST;
     blue = blue * BLUE_GRAYSCALE_CONST;
     green = green * GREEN_GRAYSCALE_CONST;

     ap_uint<8> result = red + blue + green;

     return result;

};

ap_fixed<24, 16> sumWindow(hls::Window<KERNEL_WIDTH, KERNEL_HEIGHT, pixel_data> &window){
#pragma HLS inline

	ap_fixed<24, 16> accumulator = 0;

	ap_fixed<24, 16> temp_acc = 0;

	for(int numRows = 0; numRows < 3; numRows++){
       for(int numCols = 0; numCols < KERNEL_HEIGHT; numCols++){

			temp_acc+=window.getval(numRows, numCols);
	}
  }

	accumulator = temp_acc;

	return accumulator;

};

void Canny(stream_t &IN, stream_t &OUT,  debug_idxPixel &idxPixelDebug, ap_uint<32> &processedPixelDebug, ap_uint<32> &idxColDebug, ap_uint<32> &idxRowDebug,
		   ap_uint<32> &SOFCounterDebug, bool &SOFDebug){
#pragma HLS INTERFACE axis port=IN
#pragma HLS INTERFACE axis port=OUT
#pragma HLS INTERFACE ap_none port=idxPixelDebug
#pragma HLS INTERFACE ap_none port=processedPixelDebug
#pragma HLS INTERFACE ap_none port=idxColDebug
#pragma HLS INTERFACE ap_none port=idxRowDebug
#pragma HLS INTERFACE ap_none port=SOFCounterDebug
#pragma HLS INTERFACE ap_none port=SOFDebug

#pragma HLS INTERFACE ap_ctrl_none port=return


	ap_fixed<24, 16> GAUSSIAN_KERNEL[KERNEL_HEIGHT*KERNEL_WIDTH] = {
                0.0625, 0.125, 0.0625,
			    0.125, 0.25, 0.125,
			    0.0625,  0.125, 0.0625,
	     };

	ap_int<16> SOBEL_KERNEL_X[KERNEL_HEIGHT*KERNEL_WIDTH] = {
		       -1, 0, 1,
			   -2, 0, 2,
			   -1, 0, 1,
	     };

	ap_int<16> SOBEL_KERNEL_Y[KERNEL_HEIGHT*KERNEL_WIDTH] = {
			    1, 2, 1,
				0, 0, 0,
			   -1, -2, -1,
	     };

	hls::LineBuffer<LINE_BUFF_HEIGHT, LINE_BUFF_WIDTH, raw_pixel_data> lineBuff;

	hls::LineBuffer<LINE_BUFF_HEIGHT, LINE_BUFF_WIDTH, pixel_data> gaussianLineBuff;

    // Sliding window contains the results of the convolution

	hls::Window<KERNEL_HEIGHT, KERNEL_WIDTH, pixel_data> window;

	hls::Window<KERNEL_HEIGHT, KERNEL_WIDTH, pixel_data> windowSobelX;

	hls::Window<KERNEL_HEIGHT, KERNEL_WIDTH, pixel_data> windowSobelY;

    stream24_t curr_pixel, output_pixel;

    ap_uint<8> grayscaleResult = 0;
	ap_uint<8> gaussianResult = 0;
	ap_uint<8> sobelResult = 0;

    // Index used to keep track of row and col
    int idxCol = 0;
    int idxRow = 0;
    int pixConvolved = 0;

    int TLAST = 0;

    // Index used to keep track of S.O.F
    int sofCounter = 0;
    bool SOF = false;

#ifdef DEBUG_MODE
//	// Debug signals:
    static int validOutputPix = 0;
    std::cout << "[INFO] Frame resolution has been set to 144p for debugging. Frame width is: " << FRAME_WIDTH << " and frame height is " << FRAME_HEIGHT << std::endl;
#else
    std::cout << "[INFO] Frame resolution has been set to high res (non-debugging)." << std::endl;
#endif

    int row = 0;
    int col = 0;

    int spixConvolved = 0;

    // Variables for result of Sobel filter
    ap_fixed<24, 16> Gx = 0;
    ap_fixed<24, 16> Gy = 0;
    ap_int<8> result = 0;

    // Iterate through all pixels in the 1280x720 image
    for(int idxPixel = 0; idxPixel < (FRAME_WIDTH*FRAME_HEIGHT); idxPixel++){
#pragma HLS PIPELINE II=3

    	IN.read(curr_pixel);

    	// Set TUSER for frame sync. VDMA

    	if(idxPixel == 0){

    		output_pixel.user = 1;

    	    sofCounter++;

    		SOFCounterDebug = sofCounter;

    		SOFDebug = true;

    	} else {
    		output_pixel.user = 0;

    		SOFDebug = false;
    	}

    	SOFDebug = false;

#ifdef DEBUG_MODE
        idxPixelDebug = idxPixel;
        idxColDebug = idxCol;
        idxRowDebug = idxRow;
        SOFDebug = sofCounter;
#endif

    	grayscaleResult = rgb_to_grayscale(curr_pixel);

    	curr_pixel.data.range(23, 16) = grayscaleResult;
    	curr_pixel.data.range(15, 8) = grayscaleResult;
    	curr_pixel.data.range(7, 0) = grayscaleResult;

    	lineBuff.shift_up(idxCol);
    	lineBuff.insert_top(grayscaleResult, idxCol);

    	// Gaussian convolution
    	for(int idxWinRow = 0; idxWinRow < KERNEL_HEIGHT; idxWinRow++){
    		for(int idxWinCol = 0; idxWinCol < KERNEL_WIDTH; idxWinCol++){

    			// idxWinCol + pixConvolved, will slide the window...
    			ap_fixed<24, 16> val = 0;

				ap_uint<8> temp = lineBuff.getval(idxWinRow, idxWinCol+pixConvolved);
				val.range(15, 8) = temp;

    			// multiply kernel by sampling window
    			val = GAUSSIAN_KERNEL[(idxWinRow*KERNEL_WIDTH) + idxWinCol] * val;

    			window.insert(val, idxWinRow, idxWinCol);

    		}

    	}

    	if((idxRow >= KERNEL_HEIGHT-1) && (idxCol >= KERNEL_WIDTH-1)){

    		// Find the Gaussian pixel value by summing, and store in another linebuffer:

    	   ap_fixed<24, 16> gaussianOutput = 0;

    	   gaussianOutput = sumWindow(window);

    	   gaussianLineBuff.shift_up(idxCol);
    	   gaussianLineBuff.insert_top(gaussianOutput, idxCol);

    		// Gx & Gy
    		for(int idxWinRow = 0; idxWinRow < KERNEL_HEIGHT; idxWinRow++){
    			for(int idxWinCol = 0; idxWinCol < KERNEL_WIDTH; idxWinCol++){

    				ap_fixed<24, 16> val_x = 0;
    				ap_fixed<24, 16> val_y = 0;

					val_x = gaussianLineBuff.getval(idxWinRow, idxWinCol+spixConvolved);
					val_y = val_x;

					val_x = SOBEL_KERNEL_X[(idxWinRow*KERNEL_WIDTH) + idxWinCol] * val_x;

					val_y = SOBEL_KERNEL_Y[(idxWinRow*KERNEL_WIDTH) + idxWinCol] * val_y;

					windowSobelX.insert(val_x, idxWinRow, idxWinCol);

    				windowSobelY.insert(val_y, idxWinRow, idxWinCol);

    			}

    		}

    		// Check Sobel lineBuff data
    		if((idxRow >= KERNEL_HEIGHT*2 - 2) && (idxCol >= KERNEL_WIDTH-1)){

    			Gx = sumWindow(windowSobelX);
    			Gy = sumWindow(windowSobelY);

    			ap_int<16> abs_Gx = Gx ^ (Gx >> 23);  // Invert Gx if negative
    			ap_int<16> abs_Gy = Gy ^ (Gy >> 23);
                result = abs_Gx + abs_Gy;

    			sobelResult = result;

    			output_pixel.data.range(23, 16) = sobelResult;
    			output_pixel.data.range(15, 8) = sobelResult;
    			output_pixel.data.range(7, 0) = sobelResult;

    			// Set TLAST for line sync.

    			if(idxCol == FRAME_WIDTH-1){

    			    output_pixel.last = 1;

    			    TLAST++;

    			} else {

    			    output_pixel.last = 0;
    			}

    			    output_pixel.strb = -1;
    			    output_pixel.keep = -1;
    			    output_pixel.id = 0;
    			    output_pixel.dest = 0;

    			    OUT.write(output_pixel);

    			    spixConvolved++;

    		}

    		pixConvolved++;

    	}

    	// calculate row and col index
    	if(idxCol < (FRAME_WIDTH-1)){

    		idxCol++;

    	} else {

    		// new line
    		idxCol = 0;
    		idxRow++;
    		pixConvolved = 0;
    		spixConvolved = 0;
    	}

    }

#ifdef DEBUG_MODE
    std::cout << "[INFO] Finished processing frame." << std::endl;
#endif
};
