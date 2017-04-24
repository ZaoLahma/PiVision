class PiVisionPpmImageCreator():
    def __init__(self):
        print("PiVisionPpmImageCreator called")
        

    def createPpmImage(self, rawData):
        ppmHeader = ""
        ppmHeader += "P6"
        ppmHeader += "# motion_detection.ppm"
        ppmHeader += "640 480"
        