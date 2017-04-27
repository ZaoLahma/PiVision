import picamera
import time
import io
from PiVisionNwM import PiVisionServer

class PiVisionCam:
	def __init__(self, resolution):
		self.camera = picamera.PiCamera()
		self.camera.rotation = 180
		self.resolution = resolution
		self.camera.resolution = self.resolution
		self.camera.start_preview()		
		time.sleep(2)
		
	def captureImage(self):
		currImage = io.BytesIO()
		self.camera.capture(currImage, 'rgb')
		
		return currImage.getvalue()
		

if __name__ == "__main__":
	print("PiVisionCamMain called")
	nm = PiVisionServer(3077)
	nm.waitForConnection()
	cam = PiVisionCam((640, 480))
	while True:
		image = cam.captureImage()
		nm.send(image)
		
	nm.stop()
