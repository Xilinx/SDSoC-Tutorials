
<table style="width:100%">
    <tr>
        <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC Environment Tutorial: Migrate OpenCV to xfOpenCV</h2>
        </th>
    </tr>
    <tr>
    <td align="center"><a href="README.md">Introduction</a></td>
    <td align="center"><a>Lab 1: Migrate OpenCV to xfOpenCV</a></td>
    <td align="center"><a href="lab2-build-sdsoc-acceleration-project.md">Lab 2: Build the SDSoC Acceleration Project</a></td>
    </tr>
</table>



## Lab 1 - Migrate OpenCV to xfOpenCV

This lab helps you understand how to take an OpenCV program written for a CPU and migrate it to the xfOpenCV library, making use of hardware accelerated functions on the reVision platform. 

>**:pushpin: NOTE:**
>Not all OpenCV functions are replicated in the xfOpenCV library, and some functions were written to simplify certain procedures.**

This tutorial includes source files and a test image for your use in the [source.zip](./source.zip) file. Download and extract the file to a location of your choosing. This includes the following files and folders:

* `colordetect.cpp` - C++ source file for you to edit in this tutorial. 
* `rock_landscape.jpg` - Test image for your use in testing the application. 
* `solution` folder - Source files for a completed conversion for your reference.

The source code is a very simple color detector for Blue, Green, and Orange on an input image of 1920x1080, such as the test image shown below. With the provided code and image, you will look at how to migrate the OpenCV functions and application flow using the xfOpenCV functions based on the ZCU102 reVISION platform. You should learn the following from this tutorial:

* Migrating OpenCV functions and flows to the xfOpenCV functions and flows.
* Identify functions for hardware acceleration.
* Setup the build environment for compilation of xfOpenCV code.
* Modifying code to be used on the ZCU102 reVISION platform.

Input Image:
![Rock Landscape](./images/rock_landscape.jpg)

### Step 1: Create and Edit the Source Files

With the source code open in a code editor, you can begin to convert it to run on the ZCU102 board.

1. After downloading and extracting the source files, open the `colordetect.cpp` source file in a code editor of your choice. 

    The `colordetect.cpp` file contains the the `colordetect()` function as described above, and the `main()` function for the application. In this tutorial you will create a hardware accelerated version of the colordetect function, `colordetect_accel()`, in a separate file and header file. 

2. At the top of the `colordetect.cpp` file, add the xfOpenCV include statements needed to support the reVISION platform. Replace the following three lines right below `#include <iostream>`: 

    ```c++
    #include <opencv2/opencv.hpp>
    #include <opencv2/highgui.hpp>
    #include <opencv2/imgproc.hpp>
    ```

    With the following lines of code:

    ```c++
    #if __SDSCC__
    #undef __ARM_NEON__
    #undef __ARM_NEON
    #include "opencv2/highgui/highgui.hpp"
    #include "opencv2/imgproc/imgproc.hpp"
    #include <opencv2/core/core.hpp>
    #define __ARM_NEON__
    #define __ARM_NEON
    #else
    #include "opencv2/highgui/highgui.hpp"
    #include "opencv2/imgproc/imgproc.hpp"
    #include <opencv2/core/core.hpp>
    #endif
    ```

    The first part related to `ARM_NEON` is to handle compilation of the OpenCV includes/libraries for the Arm processor on the ZCU102. Below the `ARM_NEON` section is where you are defining the xfOpenCV includes to tool is going to use. Including these files will insure the compiler has access to all functions and datatypes needed. 

    >**:pushpin: NOTE:**
    >`__SDSCC__` is automatically set by the sds++ compiler. 

3. In the same code editor you are using to edit `colordetect.cpp`, create two new files: `colordetect_accel.cpp` and `colordetect_accel.hpp`. 
   
4. In the `colordetect_accel.cpp` file define the function signature for the new hardware accelerated function. Type the following at the top of the file: 
   
    ```c++
    void colordetect_accel() {}
    ```

5. In the `colordetect_accel.hpp` header file add the following lines of code:

    ```c++
    #include "hls_stream.h"
    #include "ap_int.h"
    #include "common/xf_common.h"
    #include "common/xf_utility.h"
    #include "imgproc/xf_inrange.hpp"
    #include "imgproc/xf_rgb2hsv.hpp"
    #include "imgproc/xf_erosion.hpp"
    #include "imgproc/xf_dilation.hpp"
    ```

6. To make things more readable, and to support templating some of the xfOpenCV function calls and object instantiations, you will add the following macros to the `colordetect_accel.hpp` file, after the prior `#include` statements:

    ```c++
    #define MAXCOLORS 3
    #define WIDTH 1920
    #define HEIGHT 1080
    ```

7. Save all the files, but keep them open for further editing.

### Step 2: Convert cv:Mat objects to xf:Mat

1. When migrating OpenCV code to a hardware accelerated platform you should identify all the objects being used in the conversion code. Look in the `main()` and `colordetect()` functions and identify the following `cv::Mat` objects: `in_img`, `out_img`, `mask1`, `mask2`, `mask3`, `_imgrange`, and `_imghsv`.

    When running on a CPU, these objects allow for dynamic allocation and deallocation of memory as needed. However, when targeting hardware acceleration, memory requirements need to be determined at compile time. To solve this, the xfOpenCV library provides the `xf::Mat` object which facilates memory allocatation in the FPGA device. Any `cv::Mat` object used by a hardware accelerated function, whether user defined or coming from the xfOpenCV library, should be replaced with a `xf::Mat` equivalent object.  
    
    In the `main()` function, you can continue to use `cv::Mat` for input/output to the Linux system, because the code in this function is running on the CPU. However, you must convert the data from the `cv::Mat` object(s) to the `xf::Mat` object(s) for the accelerated function.

2. In the `main()` function, create input and output `xf::Mat` objects for the accelerated function, after the following if-statement:
   
    ```C++
    if (!in_img.data) {
	    return -1;
	}
    ```

    Add the following code:

    ```c++
    xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfIn(in_img.rows, in_img.cols);
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfOut(in_img.rows, in_img.cols);
    ```

   These statements create templated `xf::Mat` objects for the input and output of the hardware accelerated function. General information related to the templated object can be found in the *Xilinx OpenCV User Guide* ([UG1233](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)).  

   The `xfIn` and `xfOut` objects you created have the following attributes:
   
   `XF_8UC4` - Defines an 8-bit unsigned-char 4-channel datatype.

   `XF_8UC1` - Defines an 8-bit unsigned-char 1-channel datatype.

   The input `xf:Mat` is declared as a 4-channel object to allow the `colorthresholding()` function to operate on the three color components (channels) of the image. The output object does not require three channels.

   `HEIGHT`/`WIDTH` - Defines the size of the input image. In the `colordetect_accel.hpp` header file, you have defined the HEIGHT and WIDTH macros for the image as **1920x1080**. Alternatively, you could statically define the maximum image size, and process variable image sizes using function parameters. 

   `XF_NPPC1` - This template parameter tells the compiler that the number of pixels processed per clock cycle is one.

<!-- 
   In OpenCV, there is no need to create multiple `cv::Mat` objects for this type of flow, because it can handle the read/writes as needed because of the dynamic aspect of the object. In acceleration, you need to know how the data is moving, and the size of this data, because things will not run in a procedural way that is typical in most C/C++ programs. With acceleration, things will run in parallel, meaning the read/writes may occur at the same time. -->

### Step 3: Create the Accelerated Function

With the `main()` function creating the `xf::Mat` objects needed as inputs and outputs to the accelerated function, you are ready to write the accelerated function.  The hardware accelerated `colordetect_accel()` function will duplicate the `colordetect()` function using the xfOpenCV functions that are accelerated on hardware. 

With this accelerated function, you need to pass everything by reference, and not by value. Passing by reference lets these parameters act as either streaming inputs or outputs to the function, but not as both at the same time. 

The `main()` function will call both the `colordetect` function and the accelerated function, `colordetect_accel`, so that you can compare the performance between Arm processing and FPGA acceleration.

1. In the `colordetect_accel.cpp` file, edit the function signature to add parameters specific to the `xf::Mat` objects you created earlier: 

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src, xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst) {}
    ```

    Like any other datatype, you must match the template values with what you pass into the function. If you mistype a parameter, the compiler will error out.

2. Since the `colordetect_accel()` function uses the HSV colorspace, you also need to pass the proper thresholds. Add `nLowThreshold` and `nHighThreshold` to the function signature as shown below:

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {}
    ```

    Using acceleration with programmable logic, you need definitive array sizes at compile time, because the code defines an actual circuit with a fixed size. Here, you know that the threshold data is a 2D array of 3 elements in each dimension, and you will define it as such.

3. Since `xf::Mat` objects are not dynamic, you will need to declare the `xf::Mat` objects used to pass the data stream through the function. In the body of the function add the following objects:

    ```C++
    xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;
    ```

4. Now you will begin defining the operations of the `colordetect_accel` function. In the original `colordetect()` function, the first operation is the color conversion (`cv::cvtColor`) of the input image from the RGB color space to HSV. Looking through the *Xilinx OpenCV User Guide* ([UG1233](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)) you will not find a `cvtColor` function. However, in the "Color Detection" section you can see that the color detection algorithm uses four hardware functions from the xfOpenCV library, beginning with the `xf::RGB2HSV` function. You will use that function in the `colordetect_accel` function to perform the color conversion. 

    Add the `xf::RGB2HSV` function with the proper parameters to `colordetect_accel` after the other `xf::Mat` object definitions as follows:

    ```c++
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);
    ```

    As you have probably noticed, functions and objects need to be templated with specific information. This is key to making sure the right datatypes/sizes are being used. The function template must match the `xf::Mat` templates, or you will hit an assert at runtime.

5. Next in the `colordetect()` function code is thresholding and combining the resulting masks together. Again, in xfOpenCV, there is no `cv::inRange` function; however, the `xf::colorthresholding` is created for just this purpose. Referring to the "Color Thresholding" section of *Xilinx OpenCV User Guide* ([UG1233](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)), there are a few things to notice:

    - The template parameter MAXCOLORS
    - `low_thresh` and `high_thresh` are one-dimensional arrays
    - The source template has to be `XF_8UC4`.

    For this function, the input `xf::Mat` object needs to be `XF_8UC4` because you are working with 3-channel data; the `MAXCOLORS` determines how many colors you're thresholding, and in the case of this code you are using 3. Lastly, the `low_thresh` and the `high_thresh` are needed to be a 1D array. This is because depending on how big of a `MAXCOLORS` you are using, you can tailor the array for the appropriate low and high values.

    There are two ways to convert a 2D array to one dimension: at instantiation time, or via for loops. For simplicity, you will use the for loop method here. Before the `xf::RGB2HSV` function that you just added, insert the following code:

    ```c++
    unsigned char low_thresh[9];
    unsigned char high_thresh[9];

    for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j) {
        low_thresh[i*3+j] = nLowThresh[i][j];
        high_thresh[i*3+j] = nHighThresh[i][j];		
    }
    ```

    Your `colordetect_accel` function should currently look something like this:

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {

        //Define Matrix objects for operations
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;

        //Convert 2d Array to 1d
        unsigned char low_thresh[9];
        unsigned char high_thresh[9];

        for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            low_thresh[i*3+j] = nLowThresh[i][j];
            high_thresh[i*3+j] = nHighThresh[i][j];		
        }

        //Begin color conversion...
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);

    }
    ```

    Now that you have one-dimensional arrays for the thresholds, you need to add the `xf::colorthresholding` function. Add the following code after the `xf::RGB2HSV` function:

    ```c++
    xf::colorthresholding<XF_8UC4, XF_8UC1, MAXCOLORS, HEIGHT, WIDTH, XF_NPPC1>(_hsv, _range, low_thresh, high_thresh);
    ```

    With this function, you are converting the input of `XF_8UC4` dataype to the output of `XF_8UC1` (grayscale) datatype, and you are thresholding for `MAXCOLORS` of 3. In comparison to the original `colordetect()` code, you are essentially doing the same set of `cv::inRange` and bitwise `OR` operations for the mask. In more complex code where more or less colors are being detected, this will be able to scale, just adjust `MAXCOLORS` and the `low_thresh` and `high_thresh` arrays as needed.

6.  With the `cv::erode` and `cv::dilate` functions, you have to determine what morphological type is being used, and what pixel area to take into account. In this function, `xf::erode` and `xf::dilate` use `MORPH_RECT` and a 3x3 pixel. 

    To add these functions similar to the OpenCV version, insert the following lines after the `colorthresholding` function:

    ```c++
    xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_range, _erode);
    xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_erode, _dilate1);
    xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate1, _dilate2);
    xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate2, _dst);
    ```

    Notice that there is a new template parameter: `XF_BORDER_CONSTANT`. In the `cv::erode` and `cv::dilate` this parameter defines the border size, which has a default value of `BORDER_CONSTANT`. In xfOpenCV, you need to know by compile time if the border of an image is being adjusted. 
    
    Finally, you need to make sure that all the template values in this function match the data input and output. Any variations will either cause compilation errors, or runtime errors.

    Your completed function should look like this:

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {

        //Define Matrix objects for operations
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;

        //Convert 2d Array to 1d
        unsigned char low_thresh[9];
        unsigned char high_thresh[9];

        for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            low_thresh[i*3+j] = nLowThresh[i][j];
            high_thresh[i*3+j] = nHighThresh[i][j];		
        }

        //Begin color conversion...
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);
        xf::colorthresholding<XF_8UC4, XF_8UC1, MAXCOLORS, HEIGHT, WIDTH, XF_NPPC1>(_hsv, _range, low_thresh, high_thresh);
        xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_range, _erode);
        xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_erode, _dilate1);
        xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate1, _dilate2);
        xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate2, _dst);
    }
    ```

7. With the accelerator completed, you can add the function definition to the `colordetect_accel.hpp` by adding this line after the `#define`:

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src, xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst, unsigned char nLowThresh[3][3], unsigned char nHighThresh[3][3]);
    ```

8. Include the `colordetect_accel.hpp` header file in the `colordetect_accel.cpp` code file. Insert the following at the top of the file: 
   
    ```c++
    #include "colordetect_accel.hpp"
    ```

This completes the definition of the `colordetect_accel` function and header file. 

### Step 4: Update the Main() Function

It is time to add `colordetect_accel` to the `main` function. 

1. You will need to include the `colordetect_accel.hpp` to the `colordetect.cpp`. Add this at the top of the file, after the various OpenCV include statements:

    ```c++
    #include "colordetect_accel.hpp"
    ```

    Before calling the `colordetect_accel` function, you need to copy the input image, `in_img`, to the `xfIn` input object. Use an `xf::Mat` member function called `copyTo` which will copy the data from the `cv::Mat` object to the `xf::Mat` object. Do this after the instantiation of  `xfIn`. 

2.  Add the following line into `main()` after the creation of the `xfIn` object:

    ```c++
    xfIn.copyTo(in_img.data);
    ```

3. Now you can instantiate the hardware accelerated function below the `colordetect()` function:

    ```c++
    colordetect_accel(xfIn,xfOut,nLowThresh,nHighThresh);
    ```

4. In order to visually compare the outputs of processing on the ARM and programmable logic, you can copy the data from `xfOut` to a separate `cv::Mat` object using the `copyFrom` method. Add the following lines right after the `colordetect_accel` function:

    ```c++
    cv::Mat accel_out(height, width, CV_8U);
    accel_out.data = xfOut.copyFrom();
    ```

5. Save the output image using the `cv::imwrite` function. Add the following right after the other `cv::imwrite` functions:

    ```c++
    cv::imwrite("accel_out.png",accel_out);
    ```

    Your final changes should make the `main()` function look similar to the following:

    ```c++
    int main(int argc, char **argv)
    {
        //Create the input/output cv::Mat objects
        cv::Mat in_img, out_img;
        cv::Mat imghsv, imgrange, imgerode, imgdilate1, imgdilate2;

        // Define the low and high thresholds
        // Want to grab 3 colors (Blue, Green, Orange) for teh input image
        unsigned char nLowThresh[3][3] = { { 110, 150, 20 }, // Lower boundary for Blue
                                        { 38, 0, 20 }, // Lower boundary for Green
                                        { 10, 150, 20 } }; // Lower boundary for Orange
        unsigned char nHighThresh[3][3] = { { 130, 255, 255 }, // Upper boundary for Blue
                                            { 75, 125, 255 }, // Upper boundary for Green
                                            { 25, 255, 255 } }; // Upper boundary for Orange

                            // Read in the commandline for an image
        in_img = cv::imread(argv[1], 1);
        if (!in_img.data) {
            return -1;
        }

        // Create input output images for the hardware function
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfIn(in_img.rows, in_img.cols);
        xfIn.copyTo(in_img.data);
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfOut(in_img.rows, in_img.cols);

        // Create the output image to match the input image (CV_8U)
        int height = in_img.rows;
        int width = in_img.cols;
        out_img.create(height, width, CV_8U);

        // Run the input and thresholds into the colordect function
        colordetect(in_img, out_img, nLowThresh, nHighThresh);

        // Call the hardware accelerated function
        colordetect_accel(xfIn,xfOut,nLowThresh,nHighThresh);

        //Copy xfOut image to cv::Mat object
        cv::Mat accel_out(height, width, CV_8U);
        accel_out.data = xfOut.copyFrom();

        // Write out the input image and the output image
        cv::imwrite("output.png", out_img);
        cv::imwrite("input.png", in_img);
        cv::imwrite("accel_out.png",accel_out);

        return 0;
    }
    ```

6. Save and close your files.

### Conclusion

By looking at the OpenCV objects being used, and understanding how xfOpenCV templating works, you can easily identify what objects should be migrated from `cv::Mat` to `xf::Mat`, and how general functions like `xf::erode`, and complex functions like `xf::RGB2HSV` and `xf::colorthresholding` can be used to recreate the design and flow of the original OpenCV code.

Look at the original `colordetect()` and the new `colordetect_accel()` and notice that the flow is the same, but the functions used are different.

In this lab, you have taken an OpenCV function, `colordetect()`, and converted it to a hardware accelerated function, `colordetect_accel()`, using xfOpenCV functions. You have:

- Identified and modified OpenCV code to utilize the optimized xfOpenCV
- Transitioned the dynamic cv::Mat object to a templated xf:Mat object
- Call the templated xfOpenCV functions 

<hr/>

:arrow_forward:**Next Topic:**  [Lab 2 - Build SDSoC Acceleration Project](lab2-build-sdsoc-acceleration-project.md)

:arrow_backward:**Previous Topic:**  [Introduction](README.md)

---
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>
