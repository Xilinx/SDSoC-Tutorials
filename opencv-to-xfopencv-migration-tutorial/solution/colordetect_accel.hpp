#include "hls_stream.h"
#include "ap_int.h"
#include "common/xf_common.h"
#include "common/xf_utility.h"
#include "imgproc/xf_colorthresholding.hpp"
#include "imgproc/xf_rgb2hsv.hpp"
#include "imgproc/xf_erosion.hpp"
#include "imgproc/xf_dilation.hpp"

#define MAXCOLORS 3
#define WIDTH 1920
#define HEIGHT 1080
#define FILTER_SIZE 3
#define KERNEL_SHAPE XF_SHAPE_RECT
#define ITERATIONS 1

void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
						xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_hsv,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_range,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_erode,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dilate1,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dilate2,
						xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
						unsigned char nLowThresh[3][3],
						unsigned char nHighThresh[3][3],
						unsigned char accel_kernel[FILTER_SIZE*FILTER_SIZE]);
