<p align="right">
<a>English</a> | <a href="/docs-jp/README.md">日本語</a>
</p>
<table width="100%">
  <tr width="100%">
    <td align="center"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>2018.3 SDSoC™ Development Environment Tutorials</h1>
    <a href="https://github.com/Xilinx/SDSoC-Tutorials/branches/all">See other versions</a>
    </td>
 </tr>
 </table>

 <table style="width:100%">
 <tr>
 <td width="35%" align="center"><b>Tutorial</b>
 <td width="65%" align="center"><b>Description</b>
 </tr>
 <tr>
 <td align="center"><a href="./getting-started-tutorial/README.md">Getting Started</a></td>
 <td>This tutorial shows how to create an SDSoC environment platform that can be used to accelerate software functions using the SDx™ Integrated Development Environment (IDE) tool.</td>
 </tr>
 <tr>
 <td align="center"><a href="./opencv-to-xfopencv-migration-tutorial/README.md">Migrate OpenCV to xfOpenCV</a></td>
 <td>This tutorial demonstrates how to migrate from using OpenCV to using xfOpenCV, which is hardware-accelerated functions optimized for Xilinx® SoCs and FPGAs. By optimizing your code to use xfOpenCV, code is up to 40x faster than embedded GPUs and 100x faster than CPUs.</td>
 </tr>
  <tr>
 <td align="center"><a href="./platform-creation-tutorial/README.md">Platform Creation</a></td>
 <td>This tutorial demonstrates how to create an SDSoC platform on which an example SDSoC application is created and run.</td>
 </tr>
   </table>

## Overview

The SDSoC (Software-Defined System-On-Chip) environment is an Eclipse-based Integrated Development Environment (IDE) for implementing heterogeneous embedded systems using the Zynq-7000 SoC and Zynq UltraScale+ MPSoC platforms. The SDSoC environment provides an embedded C/C++ application development experience with an easy to use Eclipse IDE, and comprehensive design tools for heterogeneous Zynq SoC development to software engineers and system architects. The SDSoC environment includes a full-system optimizing C/C++/OpenCL compiler that provides automated software acceleration in programmable logic combined with automated system connectivity generation. The application programming model within the SDSoC environment should be intuitive to software engineers. An application is written as C/C++/OpenCL code, with the programmer identifying a target platform and a subset of the functions within the application to be compiled into hardware. The SDSoC system compiler then compiles the application into hardware and software to realize the complete embedded system implemented on a Zynq device, including a complete boot image with firmware, operating system, and application executable.

The SDSoC environment abstracts hardware through increasing layers of software abstraction that includes cross-compilation and linking of C/C++/OpenCL functions into programmable logic fabric as well as the Arm CPUs within a Zynq device. Based on a user specification of program functions to run in programmable hardware, the SDSoC environment performs program analysis, task scheduling and binding onto programmable logic and embedded CPUs, as well as hardware and software code generation that automatically orchestrates communication and cooperation among hardware and software components.  

The SDSoC environment 2018.2 release includes support for the ZC702, ZC706, and Zed development boards featuring the Zynq-7000 SoC, and for the ZCU102 development board featuring the Zynq UltraScale+ MPSoC. Additional platforms are available from partners. For more information, visit the [SDSoC development environment web page](https://www.xilinx.com/products/design-tools/software-zone/sdsoc.md).

<hr/>
<p align="center"><sup>Copyright&copy; 2019 Xilinx</sup></p>
