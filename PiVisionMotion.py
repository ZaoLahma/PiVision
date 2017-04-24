from PiVisionNwM import PiVisionClient
import math
import time
from PiVisionPpmImageCreator import PiVisionPpmImageCreator

class PiVisionMotion:
    def __init__(self):
        print("PiVisionMotion::init called")
        self.prevImage = None
        self.printed = False
    
    def analyzeImage(self, image):
        currImage = []
        
        if None != image:
            startTime = time.time()            
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
        if None != self.prevImage:
            byteIndex = 0
            for byte in currImage:
                diff = math.fabs(byte - self.prevImage[byteIndex])
                if diff > 100:
                    print("Diff (" + str(diff) + ") at pixel " + str(byteIndex))
                    motionFound = True
                byteIndex += 1
        if True == motionFound:
            if self.printed == False:
                self.printed = True
                imageCreator = PiVisionPpmImageCreator()
                imageCreator.createPpmImage(image)            
        if [] != currImage:
            self.prevImage = currImage

if __name__ == "__main__":
    nw = PiVisionClient()
    nw.connect("192.168.1.250", 3077)
    nw.start()
    motion = PiVisionMotion()
    while True:
        image = nw.getData()
        motion.analyzeImage(image)