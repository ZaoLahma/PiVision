# PiVision
Playing around with the Rpi camera

Aim is to create a modular system with different image processing algorithms.

On the RaspberryPi side there is a server module which is continously capturing and sending raw image data to all connected clients.

A client (might be a GUI or motion detection algorithm) can connect to the server and perform whatever operations need to be performed on
the image data and make decisions based on the result.

This is a toy project of mine so I won't be taking it too seriously in terms of code quality or functionality.

The screenshot below shows the system highlighting the movement of the picture on a TV
![image](https://user-images.githubusercontent.com/17113438/39840390-8d68bbfe-53df-11e8-92e6-bb4777ee51d5.png)
