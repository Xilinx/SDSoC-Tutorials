// colordetect.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
	// Want to grab 3 colors (Blue, Green, Orange) for teh input image
	unsigned char nLowThresh[3][3] = {	{ 110, 150, 20 }, // Lower boundary for Blue
										{ 38, 0, 20 }, // Lower boundary for Green
										{ 10, 150, 20 } }; // Lower boundary for Orange
	unsigned char nHighThresh[3][3] = { { 130, 255, 255 }, // Upper boundary for Blue
										{ 75, 125, 255 }, // Upper boundary for Green
										{ 25, 255, 255 } }; // Upper boundary for Orange

	// Setup the kernel matrix
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(FILTER_SIZE, FILTER_SIZE), cv::Point(-1,-1));

	// Read in the commandline for an image
	in_img = cv::imread(argv[1], 1);
	if (!in_img.data) {
		return -1;
	}

	// Create the output image to match the input image (CV_8U)
	int height = in_img.rows;
	int width = in_img.cols;
	out_img.create(height, width, CV_8U);

	// Run the input and thresholds into the colordect function
	colordetect(in_img, out_img, nLowThresh, nHighThresh, element);

	// Write out the input image and the output image
	cv::imwrite("output.png", out_img);
	cv::imwrite("input.png", in_img);

	return 0;
}
