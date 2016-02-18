# CAN_CAR
Project for a RC car modded to be a mockup of a CAN bus for a real car


SCAD directory contains 3D models used for the motor encoder and parts mounting platform
PCB directory contains PCB designs for the Teensy CAN adapter and Arduin pro mini CAN adapter
SW directory contains code for both the CAN peripherals and main control unit



The main brain of the car is a Beaglebone green fitted with a CAN bus cape from Waveshare. The motor and steering servo control is performed by a teensy 3.1/2 fitted with a custom CAN board. Additional control units (collision sensors, headlights ...) will be added using arduino pro mini with CAN adapter.
The goal is to easily add sub-systems on the CAN bus to add functions ilke :
- line crossing detection
- vision-based road sign reading
- ...

Car base used for this project :
http://www.hobbyking.com/hobbyking/store/__24971__Turnigy_Trooper_SCT_4x4_1_10_Brushless_Short_Course_Truck_ARR_.html

The brushless motor and ESC are removed and replaced with a brushed 540 motor with H-bridge (easier to get full control of the motor)

H-bridge driver for the brushed motor

https://www.pololu.com/product/706 (cheaper eBay equivalent)


