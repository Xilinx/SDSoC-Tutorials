#include "colordetect_accel.hpp"

void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
						xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_hsv,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_range,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_erode,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dilate1,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dilate2,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
						unsigned char nLowThresh[3][3],
						unsigned char nHighThresh[3][3],
						unsigned char accel_kernel[FILTER_SIZE*FILTER_SIZE]) {

	unsigned char low_thresh[9];
	unsigned char high_thresh[9];

	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j) {
			low_thresh[i*3+j] = nLowThresh[i][j];
			high_thresh[i*3+j] = nHighThresh[i][j];
		}

	xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);
	xf::colorthresholding<XF_8UC4, XF_8UC1, MAXCOLORS, HEIGHT, WIDTH, XF_NPPC1>(_hsv, _range, low_thresh, high_thresh);

	xf::erode<XF_BORDER_REPLICATE, XF_8UC1, HEIGHT, WIDTH, XF_SHAPE_RECT, FILTER_SIZE, FILTER_SIZE, ITERATIONS, XF_NPPC1>(_range, _erode, accel_kernel);
	xf::dilate<XF_BORDER_REPLICATE, XF_8UC1, HEIGHT, WIDTH, XF_SHAPE_RECT, FILTER_SIZE, FILTER_SIZE, ITERATIONS, XF_NPPC1>(_erode, _dilate1, accel_kernel);
	xf::dilate<XF_BORDER_REPLICATE, XF_8UC1, HEIGHT, WIDTH, XF_SHAPE_RECT, FILTER_SIZE, FILTER_SIZE, ITERATIONS, XF_NPPC1>(_dilate1, _dilate2, accel_kernel);
	xf::erode<XF_BORDER_REPLICATE, XF_8UC1, HEIGHT, WIDTH, XF_SHAPE_RECT, FILTER_SIZE, FILTER_SIZE, ITERATIONS, XF_NPPC1>(_dilate2, _dst, accel_kernel);
}
