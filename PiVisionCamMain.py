from PiVisionNwM import PiVisionNwM
from PiVisionCam import PiVisionCam

if __name__ == "__main__":
	print("PiVisionCamMain called")
	nm = PiVisionNwM(3077)
	nm.waitForConnection()
	cam = PiVisionCam((640, 480))
	while True:
		image = cam.captureImage()
		nm.send(image)
		
	nm.stop()
