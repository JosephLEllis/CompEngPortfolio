#include "hls_opencv.h"
#include <opencv2/opencv.hpp>

#include <iostream>

#include "D:\PYNQ_CUSTOM_IPS_VIVADO_2019\Canny\Canny\HLS\Canny.h"

int currentRow = 0;

#define FRAME_WIDTH_HD 1280
#define FRAME_HEIGHT_HD 720

#define FRAME_WIDTH_LD 256
#define FRAME_HEIGHT_LD 144

#define TEST_HDLD 0

int main(){

  cv::Mat lowres, highres;

  if(TEST_HDLD == 1){

	  highres = cv::imread("D:/PYNQ_CUSTOM_IPS_VIVADO_2019/Canny/Canny/Testbenches/garfield.jpg", cv::IMREAD_COLOR);

	  if(highres.empty()){
		  	  std::cout << "Error! Could not open high res image!" << std::endl;

		  	  return -1;
	  } else {
		  std::cout << "Loaded high res image successfully" << std::endl;
	  }

  } else {
	  lowres = cv::imread("D:/PYNQ_CUSTOM_IPS_VIVADO_2019/Canny/Canny/Testbenches/LowRes/garfield_lowres.png", cv::IMREAD_COLOR);

	  if(lowres.empty()){
			  	  std::cout << "Error! Could not open low res image!" << std::endl;

			  	  return -1;
		  } else {
			  std::cout << "Loaded low res image successfully" << std::endl;
		  }

  }

  stream_t input_stream, output_stream;

  stream24_t output_pix;

  debug_idxPixel idxPix = 0;
  ap_uint<32> processedPix = 0;
  ap_uint<32> idxCol = 0;
  ap_uint<32> idxRow = 0;

  ap_uint<32> SOFCounter = 0;
  bool SOF = false;

  int width, height;

  if(TEST_HDLD == 1){

	  width = highres.cols;
	  height = highres.rows;
  } else {
	  std::cout << "[INFO] Using lowres for testbench." << std::endl;
	  width = lowres.cols;
	  height = lowres.rows;
  }

  cv::Mat output_image(height, width, CV_8UC3);
  cv::Mat grayscale_output;
  cv::Mat output;

  if(TEST_HDLD == 1){
	  cv::cvtColor(highres, grayscale_output, cv::COLOR_BGR2GRAY);
  } else {
	  cv::cvtColor(lowres, grayscale_output, cv::COLOR_BGR2GRAY);
  }

   cv::Mat blurred;
   cv::GaussianBlur(grayscale_output, blurred, cv::Size(3, 3), 0); // 3x3 kernel

   cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y, sobel_output;

   // Gradient X
   cv::Sobel(blurred, grad_x, CV_16S, 1, 0, 3);
   cv::convertScaleAbs(grad_x, abs_grad_x);

   // Gradient Y
   cv::Sobel(blurred, grad_y, CV_16S, 0, 1, 3);
   cv::convertScaleAbs(grad_y, abs_grad_y);

   // Combine X and Y gradients
   cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobel_output);

  // Populate input stream with data from image:

  for(int numRows = 0; numRows < height; numRows++){

	  for(int numCols = 0; numCols < width; numCols++){

		  stream24_t input_pixel;

		  cv::Vec3b colour_val;

		  if(TEST_HDLD == 1){

			 colour_val = highres.at<cv::Vec3b>(numRows, numCols);

		  } else {
			  colour_val = lowres.at<cv::Vec3b>(numRows, numCols);
		  }

		  uchar red = colour_val.val[2];
		  uchar green = colour_val.val[1];
		  uchar blue = colour_val.val[0];

		  input_pixel.data.range(23, 16) = red; // RBG
		  input_pixel.data.range(15, 8) = blue;
		  input_pixel.data.range(7, 0) = green;

		  if(numCols == width-1){

			 input_pixel.last = 1;


		  } else {

			  input_pixel.last = 0;
		  }


		  input_stream.write(input_pixel);

	  }

  }

  Canny(input_stream, output_stream, idxPix, processedPix, idxCol, idxRow, SOFCounter, SOF);

      int row = 0;
      int col = 0;
      int numTLAST = 0;

      while(!output_stream.empty()){

    	  output_stream.read(output_pix);

    	  uchar red = output_pix.data.range(23, 16).to_uint();

    	  uchar green = output_pix.data.range(15, 8).to_uint();

    	  uchar blue = output_pix.data.range(7, 0).to_uint();

    	  if(row < height && col < width){
    		  output_image.at<cv::Vec3b>(row, col) = cv::Vec3b(red, blue, green);
    	  }

    	  if(output_pix.last){

    		  col = 0;
    		  row++;

    		  numTLAST++;

    	  } else {
    		  col++;
    	  }

      }

      std::cout << "TLAST was set: " << numTLAST << " times." << std::endl;
      std::cout << "TUSER was set: " << SOFCounter << " times " << std::endl;

      std::cout << "The IP output: " << processedPix << " valid pixels." << std::endl;

      FrameTest(width, height, processedPix);

  // Show the original and processed images using OpenCV

      if(TEST_HDLD == 1) {
    	  cv::imshow("Original Image", highres);
      } else {
    	  cv::imshow("Original Image", lowres);
      }


      cv::waitKey(0);

      cv::imshow("Correct output", sobel_output);

      cv::waitKey(0);

      cv::imshow("Processed Image (Grayscale)", output_image);

      cv::waitKey(0);

};

