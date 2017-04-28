from PiVisionNwM import PiVisionClient
from PiVisionNwM import PiVisionServer
import PiVisionConstants
import threading

class PiVisionGrayscaleServer(threading.Thread):
	def __init__(self):
		print("PiVisionGrayscaleServer::init called")
		threading.Thread.__init__(self)
		self.nm = PiVisionClient(PiVisionConstants.IMAGE_BYTE_SIZE)
		self.nm.connect("192.168.1.250", PiVisionConstants.COLOR_SERVICE)
		self.nm.start()
		self.running = False
	
	def run(self):
		self.running = True
		self.server = PiVisionServer(PiVisionConstants.GRAYSCALE_SERVICE)
		while True == self.running:
			image = self.nm.getData()
			if None != image:
				print("Creating grayscale image")
				grayScaleImage = []
				byteCounter = 0
				pixelVal = 0
				for byte in image:
					pixelVal += ord(byte)
					byteCounter += 1
					if 3 == byteCounter:
						byteCounter = 0
						grayScaleImage.append(pixelVal/3)
						pixelVal = 0
				self.server.send(grayScaleImage)
			
			
if "__main__" == __name__:
	grayscaleServer = PiVisionGrayscaleServer()
	grayscaleServer.run()
