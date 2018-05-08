#!/usr/bin/python

from PiVisScheduler import PiVisScheduler
from PiVisServer import PiVisServer
import PiVisConstants
import picamera
import time
import io
import sys

class PiVisCamera:
    def __init__(self, scheduler, server, resolution):
        self.server = server
        self.camera = picamera.PiCamera()
        self.camera.rotation = 180
        self.resolution = resolution
        self.camera.resolution = self.resolution
        time.sleep(2)
        scheduler.registerRunnable(self.run)

    def run(self):
        currImage = io.BytesIO()
        self.camera.capture(currImage, 'rgb', use_video_port=True)
        self.server.send(bytearray(currImage.getvalue()))

if __name__ == "__main__":
    print("PiVisCamera starting")
    scheduler = PiVisScheduler()
    if len(sys.argv) > 0:
        server = PiVisServer(scheduler, PiVisConstants.CAMERA_SERVICE)
    else:
        server = PiVisServer(scheduler, PiVisConstants.RAW_IMAGE_SERVICE)
    camera = PiVisCamera(scheduler, server, PiVisConstants.IMAGE_RESOLUTION)
    scheduler.run()
