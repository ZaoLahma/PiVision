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
        self.camera.capture(currImage, 'rgb', use_video_port=True)
        imageData = bytearray()
        if self.appendRes:
            xRes = self.resolution[0]
            yRes = self.resolution[1]
            imageData.append(xRes.to_bytes(2, byteorder='little'))
            imageData.append(yRes.to_bytes(2, byteorder='little'))
        imageData.append(currImage.getvalue())
        self.server.send(imageData)

if __name__ == "__main__":
    print("PiVisCamera starting")
    scheduler = PiVisScheduler()
    if len(sys.argv) > 1:
        print("Starting Camera service")
        server = PiVisServer(scheduler, PiVisConstants.CAMERA_SERVICE, PiVisConstants.DISCOVER_CAMERA_SERVICE)
    else:
        print("Starring legacy image service")
        server = PiVisServer(scheduler, PiVisConstants.RAW_IMAGE_SERVICE)
    camera = PiVisCamera(scheduler, server, PiVisConstants.IMAGE_RESOLUTION)
    scheduler.run()
