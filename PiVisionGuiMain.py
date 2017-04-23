from PiVisionGui import PiVisionGui
from PiVisionGui import PiVisionGuiThread
from PiVisionNwM import PiVisionClient

if __name__ == "__main__":
	print("PiVisionGuiMain called")
	nm = PiVisionClient()
	nm.connect("192.168.1.250", 3077)
	nm.start()	
	resolution = (480, 640)
	gui = PiVisionGui(resolution)
	guiThread = PiVisionGuiThread(nm, gui)
	guiThread.start()
	gui.mainLoop()
	guiThread.stop()
	nm.stop()
