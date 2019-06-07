// colordetect.cpp : Defines the entry point for the console application.
//

#include "colordetect.hpp"
#include <iostream>
#if __SDSCC__
#undef __ARM_NEON__
#undef __ARM_NEON
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs/imgcodecs.hpp"
#define __ARM_NEON__
#define __ARM_NEON
#else
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#endif

#include "sds_lib.h"

unsigned long clock_start, clock_end;
#define TIME_STAMP_INIT clock_start = sds_clock_counter();
#define TIME_STAMP  { clock_end = sds_clock_counter(); printf("elapsed time %lu \nelapsed real-time %f ms \n", clock_end-clock_start, (1000.0/sds_clock_frequency())*(double)(clock_end-clock_start)); clock_start = sds_clock_counter();  }

// colordetect
// Description:
// Will detect the colors from the thresholds provided
// Inputs:
//	- in_img
//  - nLowThresh
//  - nHighThresh
//  - _kernel
// Output:
//  - out_img

void colordetect(cv::Mat &_src,
				cv::Mat &_dst,
				unsigned char nLowThresh[3][3],
				unsigned char nHighThresh[3][3],
				cv::Mat &_kernel) {

	// Temporary matrices for processing
	cv::Mat mask1, mask2, mask3, _imgrange, _imghsv;

	// Convert the input to the HSV colorspace. Using BGR here since it is the default of OpenCV.
	// Using RGB yields different results, requiring a change of the threshold ranges
	cv::cvtColor(_src, _imghsv, cv::COLOR_BGR2HSV);

	// Get the color of Yellow from the HSV image and store it as a mask
	cv::inRange(_imghsv, cv::Scalar(nLowThresh[0][0], nLowThresh[0][1], nLowThresh[0][2]), cv::Scalar(nHighThresh[0][0], nHighThresh[0][1], nHighThresh[0][2]), mask1);

	// Get the color of Green from the HSV image and store it as a mask
	cv::inRange(_imghsv, cv::Scalar(nLowThresh[1][0], nLowThresh[1][1], nLowThresh[1][2]), cv::Scalar(nHighThresh[1][0], nHighThresh[1][1], nHighThresh[1][2]), mask2);

	// Get the color of Red from the HSV image and store it as a mask
	cv::inRange(_imghsv, cv::Scalar(nLowThresh[2][0], nLowThresh[2][1], nLowThresh[2][2]), cv::Scalar(nHighThresh[2][0], nHighThresh[2][1], nHighThresh[2][2]), mask3);

	// Bitwise OR the masks together (adding them) to the range
	_imgrange = mask1 | mask2 | mask3;

	// First erode
	cv::erode(_imgrange, _dst, _kernel);
	// First dilate
	cv::dilate(_dst, _dst, _kernel);
	// Second dilate
	cv::dilate(_dst, _dst, _kernel);
	// Second erode
	cv::erode(_dst, _dst, _kernel);
}

int main(int argc, char **argv)
{
	//Create the input/output cv::Mat objects
	cv::Mat in_img, out_img;
	cv::Mat imghsv, imgrange, imgerode, imgdilate1, imgdilate2;

	// Define the low and high thresholds
	// Want to grab 3 colors (Yellow, Green, Red) for the input image
	unsigned char nLowThresh[3][3] = { { 22, 150, 60 }, // Lower boundary for Yellow
	{ 38, 150, 60 }, // Lower boundary for Green
	{ 160, 150, 60 } }; // Lower boundary for Red
	unsigned char nHighThresh[3][3] = { { 38, 255, 255 }, // Upper boundary for Yellow
	{ 75, 255, 255 }, // Upper boundary for Green
	{ 179, 255, 255 } }; // Upper boundary for Red

	unsigned char accel_kernel[FILTER_SIZE*FILTER_SIZE] = {0};

	// Read in the commandline for an image
	in_img = cv::imread(argv[1], 1);
	if (!in_img.data) {
		return -1;
	}

	xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfIn(in_img.rows, in_img.cols);
	xfIn.copyTo(in_img.data);
	xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfOut(in_img.rows, in_img.cols);

	xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfHSV(in_img.rows, in_img.cols);
	xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfRange(in_img.rows, in_img.cols);
	xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfErode(in_img.rows, in_img.cols);
	xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfDilate1(in_img.rows, in_img.cols);
	xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfDilate2(in_img.rows, in_img.cols);

	// Setup the kernel matrix
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(FILTER_SIZE, FILTER_SIZE), cv::Point(-1,-1));

	for(int i = 0; i < (FILTER_SIZE*FILTER_SIZE); i++)
		accel_kernel[i] = element.data[i];

	// Create the output image to match the input image (CV_8U)
	int height = in_img.rows;
	int width = in_img.cols;
	out_img.create(height, width, CV_8U);

	// Run the input and thresholds into the colordectect function

	std::cout << "CPU" << std::endl;
	TIME_STAMP_INIT
	colordetect(in_img, out_img, nLowThresh, nHighThresh, element);
	TIME_STAMP

	std::cout << "Accelerator" << std::endl;
	TIME_STAMP_INIT
	colordetect_accel(xfIn,xfHSV,xfRange,xfErode,xfDilate1,xfDilate2,xfOut,nLowThresh,nHighThresh, accel_kernel);
	TIME_STAMP

	cv::Mat accel_out(height, width, CV_8U);
	accel_out.data = xfOut.copyFrom();

	// Write out the input image and the output image
	cv::imwrite("accel_out.png",accel_out);
	cv::imwrite("output.png", out_img);
	cv::imwrite("input.png", in_img);
	return 0;
}