from NetworkManager import NetworkManager
from RpiCamManager import RpiCamManager
from PiVisionGui import PiVisionGui

if __name__ == "__main__":
	print("Main called")
	nm = NetworkManager(3077)
	#nm.waitForConnection()
	cam = RpiCamManager((640, 480))
	image = cam.captureImage()
	resolution = (480, 640)
	gui = PiVisionGui(resolution)
	gui.showImage(image)
	gui.mainLoop()
