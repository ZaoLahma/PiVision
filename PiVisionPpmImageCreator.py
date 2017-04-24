class PiVisionPpmImageCreator():
    def __init__(self):
        print("PiVisionPpmImageCreator called")
        

    def createPpmImage(self, rawData):
        ppmImage = ""
        ppmImage += "P6\n"
        ppmImage += "# motion_detection.ppm\n"
        ppmImage += "640 480\n"
        ppmImage += "255\n"
        
        x = 0
        
        for byte in rawData:
            ppmImage += str(ord(byte))
            x += 1
            if x == 640:
                x = 0
                ppmImage += "\n"
            else:
                ppmImage += " "
                
        ppmImageFile = open('motion_detection.ppm', 'w')
        ppmImageFile.write(ppmImage)
        ppmImageFile.close()