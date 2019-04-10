<!-- <div style="page-break-after: always;"></div> -->
<!-- <div style="display: none;" media="print"> -->
<table style="width:100%">
    <tr>
        <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC Environment Tutorial: Migrate OpenCV to xfOpenCV</h2>
        </th>
    </tr>
    <tr>
    <td align="center"><a >Introduction</a></td>
    <td align="center"><a href="lab-1-migrate-opencv-to-xfopencv.md">Lab 1: Migrate OpenCV to xfOpenCV</a></td>
    <td align="center"><a href="lab2-build-sdsoc-acceleration-project.md">Lab 2: Build the SDSoC Acceleration Project</a></td>
    </tr>
</table>
<!-- </div> -->

# Introduction

xfOpenCV is hardware-accelerated OpenCV functions optimized for Xilinx SoCs and FPGAs. The functions are written completely in C/C++ that is targeted for High-level Synthesis (HLS).  This lab is designed to be used with SDx 2018.2 and using the released ZCU102 reVISION Platform.

The major difference between OpenCV and xfOpenCV is that typical OpenCV is written for and run on CPUs (x86, Arm, etc), whereas xfOpenCV is written and optimized for running on Xilinx SoCs and FPGAs. By optimizing for SoCs and FPGAs the code is up to 40x faster than embedded GPUs and 100x faster than CPUs. Since all the code is written in C/C++ it is easier to customize to your own computer vision functions.

**:pushpin: NOTE**
**This tutorial assumes you are already familiar with OpenCV and the reVISION platform as described at [reVISION Getting Started Guide](https://github.com/Xilinx/reVISION-Getting-Started-Guide/blob/master/Docs/software-tools-system-requirements.md). You may want to review this information before proceeding.**

## Revision History
This is the initial release of the tutorial.

## Change Log
### 2018.2
Initial Release

## Support
To obtain technical support for the tutorial design, go to the:

* [Xilinx Community Forums](https://forums.xilinx.com/)  to ask questions or discuss technical details and issues. Please make sure to browse the existing topics first before filing a new topic. If you do need to file a new topic, make sure it is filed in the sub-forum that best describes the issue or question and that it refers to the tutorial specifically.

<hr/>

:arrow_forward:**Next Topic:**  [Lab 1 - Migrate OpenCV to xfOpenCV](lab-1-migrate-opencv-to-xfopencv.md)

<hr/>
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>
