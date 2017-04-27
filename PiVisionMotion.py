from PiVisionNwM import PiVisionClient
import math
import time
from PiVisionPpmImageCreator import PiVisionPpmImageCreator
from PiVisionGui import PiVisionGui
import threading

class PiVisionMotion(threading.Thread):
    def __init__(self, nw):
        threading.Thread.__init__(self)
        print("PiVisionMotion::init called")
        self.prevImage = None
        self.printed = False
        self.gui = PiVisionGui((480, 640))
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
        #Super simple motion detection based off pure comparison between pixels. Will be refactored.
        motionFound = False       
        numDiffs = 0            
        if None != self.prevImage:
            byteIndex = 0
            for byte in currImage:
                diff = math.fabs(byte - self.prevImage[byteIndex])
                if diff > 50:
                    print("Diff (" + str(diff) + ") at pixel " + str(byteIndex))
                    motionFound = True
                    numDiffs += 1
                    diffImage.append(chr(255))
                    diffImage.append(chr(255))
                    diffImage.append(chr(255))
                else:
                    diffImage.append(chr(0))
                    diffImage.append(chr(0))
                    diffImage.append(chr(0))
                byteIndex += 1
            self.gui.showImage(diffImage)
        if [] != currImage:
            self.prevImage = currImage

if __name__ == "__main__":
    nw = PiVisionClient()
    nw.connect("192.168.1.250", 3077)
    nw.start()
    motion = PiVisionMotion(nw)
    motion.start()
    motion.gui.mainLoop()