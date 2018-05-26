#!/usr/bin/python

from PiVisScheduler import PiVisScheduler
from PiVisServer import PiVisServer
import PiVisConstants
import picamera
import time
import io
import sys

class PiVisCamera:
    def __init__(self, scheduler, server, resolution, appendRes=False):
        self.server = server
        self.camera = picamera.PiCamera()
        self.camera.rotation = 180
        self.resolution = resolution
        self.camera.resolution = self.resolution
        self.appendRes = appendRes
        time.sleep(2)
        scheduler.registerRunnable(self.run)

    def run(self):
        currImage = io.BytesIO()
        xRes = self.resolution[0]
        yRes = self.resolution[1]
        currImage.write(xRes.to_bytes(2, byteorder='little'))
        currImage.write(yRes.to_bytes(2, byteorder='little'))
        self.camera.capture(currImage, 'rgb', use_video_port=True)
        self.server.send(bytearray(currImage.getvalue()))

if __name__ == "__main__":
    print("PiVisCamera starting")
    scheduler = PiVisScheduler()
    camera = 0
    print("Starting Camera service")
    server = PiVisServer(scheduler, PiVisConstants.CAMERA_SERVICE, PiVisConstants.DISCOVER_CAMERA_SERVICE)
    camera = PiVisCamera(scheduler, server, PiVisConstants.IMAGE_RESOLUTION, True)

    scheduler.run()
