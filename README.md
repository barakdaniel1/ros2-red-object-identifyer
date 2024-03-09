# Ros2 Humble - Red Object Identifyer

This project uses a usb camera (laptop camera also works), identifies red objects, draws a circle around the object,  
and also draws an X in the center of it.

## Table of Contents

- [Installation](#installation)
- [Run](#run)



## Installation

First, use this guide to install ros2 humble version:  
[https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html](https://docs.ros.org/en/humble/Installation.html)  

Specifically, I used the Debian installation.  

Then, install usb_cam package with this guide:  
https://index.ros.org/p/usb_cam/  
**NOTE:** If you decide to build from source, change the cloning command to the following:  
git clone https://github.com/ros-drivers/usb_cam.git -b ros2  

Next, you should calibtrate your camera, please use this guide:   
[https://medium.com/starschema-blog/offline-camera-calibration-in-ros-2-45e81df12555](https://navigation.ros.org/tutorials/docs/camera_calibration.html)  

## Run
Open 3 terminals in the ros2 ws.   
1st terminal:   
ros2 run usb_cam usb_cam_node_exe   

2nd terminal:  
ros2 run obj_ident red_object_ident   

3rd terminal:  
ros2 run rqt_image_view rqt_image_view   

In the 3rd terminal, you can choose which video you want to see.  
Pick the red object one.  
