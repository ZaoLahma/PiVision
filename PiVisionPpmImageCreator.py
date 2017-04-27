import PiVisionConstants

class PiVisionPpmImageCreator():
    def __init__(self):
        print("PiVisionPpmImageCreator called")
        

    def createPpmImage(self, rawData):
        ppmImageHeader = ""
        ppmImageHeader += "P6 "
        ppmImageHeader += str(PiVisionConstants.IMAGE_RESOLUTION[0]) 
        ppmImageHeader += " "
        ppmImageHeader += str(PiVisionConstants.IMAGE_RESOLUTION[1])
        ppmImageHeader += " 255\n"
        retVal = bytearray(ppmImageHeader)
        for byte in rawData:
            retVal.append(byte)
        return retVal
        
        
if __name__ == "__main__":
    data = []
    
    for index in range(0, PiVisionConstants.IMAGE_BYTE_SIZE):
        data.append(255)
    
    imageCreator = PiVisionPpmImageCreator()
    
    data = imageCreator.createPpmImage(data)
    
    print("data: " + str(data))
    
    ppmImageFile = open('motion_detection_test.ppm', 'w')
    
    ppmImageFile.write(data)
    ppmImageFile.close()
