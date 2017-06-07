#!/usr/bin/python

import picamera
import time
import io

class PiVisCamera:
    def __init__(self, scheduler, server, resolution):
        self.server = server        
        self.camera = picamera.PiCamera()
        self.camera.rotation = 180
        self.resolution = resolution
        self.camera.resolution = self.resolution
        self.camera.start_preview()        
        time.sleep(2)
        scheduler.registerRunnable(self.run) 

    def run(self):
        currImage = io.BytesIO()
        self.camera.capture(currImage, 'rgb')
        self.server.send(bytearray(currImage.getvalue()))