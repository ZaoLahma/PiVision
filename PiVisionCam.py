import picamera
import time
import io

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
		
