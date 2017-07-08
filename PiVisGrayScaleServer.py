#!/usr/bin/python

from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient
from PiVisServer import PiVisServer
import PiVisConstants

class PiVisGrayScaleServer:
    def __init__(self, scheduler, client, server):
        self.client = client
        self.server = server
        scheduler.registerRunnable(self.run)
        
    def run(self):
        image = client.getData()
        pixelOffset = 0
        byteVal = 0
        grayScaleImage = bytearray()
        for byte in image:
            byteVal += byte
            pixelOffset += 1
            if pixelOffset == 3:
                byteVal = byteVal / 3
                grayScaleImage.append(int(byteVal))
                pixelOffset = 0
                byteVal = 0           
        server.send(grayScaleImage)
                
        
        
if __name__ == "__main__":
    scheduler = PiVisScheduler()
    client = PiVisClient(scheduler, 
                         PiVisConstants.RAW_IMAGE_SERVICE, 
                         PiVisConstants.IMAGE_BYTE_SIZE)
    server = PiVisServer(scheduler, PiVisConstants.IMAGE_DATA_SERVICE)
    grayScaleServer = PiVisGrayScaleServer(scheduler, client, server)
    
    scheduler.run()