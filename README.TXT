See http://ledpixelart.com/developers.html for instructions to write your own app for PIXEL.

_________________________________________________________________________
FIRMWARE INFO - Most developers will not need to touch the firmware and you can ignore this section. You only need to worry about the firmware if you're making a
fundamental change to how PIXEL works. This is a fork of the RGB-LED branch of the IOIO repository on Github https://github.com/ytai/ioio/wiki. The 
major changes from the RGB-LED branch are as follows:

1. Bluetooth name is changed from IOIO to PIXEL
2. The IOIO Loop frequency is changed from 1 Khz to 30 hz (ie, 30 times a second)
3. On soft reset, the pins used by the LED matrix (see pins below) are not reset. This is to keep the matrix going after the Android device has been disconnected.


For reference, here's the IOIO pins used by PIXEL. You only care about this however if you're doing a firmware modification. The 

Pixel's firmware is a modified version of the standard IOIO board app firmware which was developed by Ytai Ben-Tsvi specifically to interface with the LED matrix hardware used by Pixel.

Below are the IOIO pins used by Pixel.  for an explanation of IOIO Pins.

IOIO Pins used for LED Matrix: In this diagram the female connector is depicted.

      +-----+
 23   | o o |   24
      | o o |   22
 20   | o o |   21
      | o o |   19
 10   | o o |   7
      | o o |   11
 27   | o o |   25
 GND  | o o |   28
      +-----+
IOIO Pins used for Grove/Expansion Ports:

3.3V Grove Port Pins: 31 (taken by proximity sensor), 32 (open), 33 (open), 34 (open)

5V Tolerant Grove Port Pins: 3 (open), 4 (open), 5 (open), 6 (open)

Pulse Sensor Female Header 3.3V: 35 (open)

