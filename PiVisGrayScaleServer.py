#!/usr/bin/python

from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient
from PiVisServer import PiVisServer
import PiVisConstants

class PiVisGrayScaleServer:
    def __init__(self, scheduler, client, server):
        self.client = client
        self.server = server
        scheduler.register(self.run)
        
    def run(self):
        print("PiVisGrayScaleServer run called")
        image = client.getData()
        pixelOffset = 0
        byteVal = 0
        grayScaleImage = []
        for byte in image:
            byteVal += byte
            pixelOffset += 1
            if pixelOffset == 3:
                byteVal = byteVal / 3
                grayScaleImage.appen(byteVal)
                pixelOffset = 0
                byteVal = 0              
        server.send(grayScaleImage)
                
        
        
if __name__ == "__main__":
    scheduler = PiVisScheduler()
    client = PiVisClient(scheduler, 
                         PiVisConstants.COLOR_SERVICE, 
                         PiVisConstants.IMAGE_BYTE_SIZE)
    server = PiVisServer(scheduler, PiVisConstants.GRAYSCALE_SERVICE)   
    
    scheduler.run()