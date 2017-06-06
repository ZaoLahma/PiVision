#!/usr/bin/python

from PiVisionNwM import PiVisionClient
import math
from PiVisionPpmImageCreator import PiVisionPpmImageCreator
from PiVisionGui import PiVisionGui
import threading
import PiVisionConstants

class PiVisionMotion(threading.Thread):
    def __init__(self, nw):
        threading.Thread.__init__(self)
        print("PiVisionMotion::init called")
        self.prevImage = None
        self.printed = False
        self.gui = PiVisionGui(PiVisionConstants.IMAGE_RESOLUTION)
        self.nw = nw
        self.running = False
    
    def run(self):
        self.running = True
        while True == self.running:
            image = self.nw.getData()
            self.analyzeImage(image)  
    
    def stop(self):
        self.running = False      
    
    def analyzeImage(self, image):
        currImage = []
        diffImage = []
        if None != image:          
            pixelVal = 0
            pixelCounter = 0
            for byte in image:
                pixelVal += ord(byte)
                pixelCounter += 1
                if 3 == pixelCounter:
                    currImage.append(pixelVal)
                    pixelVal = 0
                    pixelCounter = 0
        #Super simple motion detection based on pure comparison between pixels. Will be refactored.
        motionFound = False     
        numDiffs = 0            
        if None != self.prevImage:
            byteIndex = 0
            for byte in currImage:
                diff = math.fabs(byte - self.prevImage[byteIndex])
                if diff > 50:
                    motionFound = True
                    numDiffs += 1
                    diffImage.append(chr(245))
                    diffImage.append(chr(64))
                    diffImage.append(chr(64))
                else:
                    diffImage.append(chr(0))
                    diffImage.append(chr(0))
                    diffImage.append(chr(0))
                byteIndex += 1
            self.gui.showImage(diffImage)
        if True == motionFound:
            if False == self.printed and numDiffs > 100:
                self.printed = True
                imageCreator = PiVisionPpmImageCreator()
                toPrint = imageCreator.createPpmImage(image)
                imageFile = open('motion_detection.ppm', 'w')
                imageFile.write(toPrint)
                imageFile.close()
                toPrint = imageCreator.createPpmImage(diffImage)
                imageFile = open('motion_detection_diff.ppm', 'w')
                imageFile.write(toPrint)
                imageFile.close()
        if [] != currImage:
            self.prevImage = currImage

if __name__ == "__main__":
    nw = PiVisionClient(PiVisionConstants.IMAGE_BYTE_SIZE)
    nw.connect("192.168.1.106", PiVisionConstants.COLOR_SERVICE)
    nw.start()
    motion = PiVisionMotion(nw)
    motion.start()
    motion.gui.mainLoop()
