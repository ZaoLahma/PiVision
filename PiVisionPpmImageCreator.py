class PiVisionPpmImageCreator():
    def __init__(self):
        print("PiVisionPpmImageCreator called")
        

    def createPpmImage(self, rawData):
        ppmImageHeader = ""
        ppmImageHeader += "P6\n"
        ppmImageHeader += "# motion_detection.ppm\n"
        ppmImageHeader += "640 480\n"
        ppmImageHeader += "255\n"
        retVal = bytearray(ppmImageHeader)
        for byte in rawData:
            retVal.append(byte)
        return retVal
        
        
if __name__ == "__main__":
    data = [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]
    imageCreator = PiVisionPpmImageCreator()
    
    data = imageCreator.createPpmImage(data)
    
    print("data: " + str(data))
    
    ppmImageFile = open('motion_detection_test.ppm', 'w')
    
    ppmImageFile.write(data)
    ppmImageFile.close()