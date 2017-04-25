class PiVisionPpmImageCreator():
    def __init__(self):
        print("PiVisionPpmImageCreator called")
        

    def createPpmImage(self, rawData):
        ppmImage = ""
        ppmImage += "P6\n"
        ppmImage += "# motion_detection.ppm\n"
        ppmImage += "640 480\n"
        ppmImage += "255\n"
        
        ppmImageData = []
        
        for byte in rawData:
            ppmImageData.append(ord(byte))
                
        ppmImageFile = open('motion_detection.ppm', 'w')
        ppmImageFile.write(ppmImage)
        ppmImageFile.write(bytearray(ppmImageData))
        ppmImageFile.close()