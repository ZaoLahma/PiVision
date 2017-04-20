from PiVisionGui import PiVisionGui
from PiVisionNwM import PiVisionNwM
import threading

class GuiThread(threading.Thread):
	def __init__(self, nwThread, gui):
		threading.Thread.__init__(self)
		self.nwThread = nwThread
		self.gui = gui
		
	def run(self):
		self.running = True
		while True == self.running:
			image = self.nwThread.getCurrImage()
			self.gui.showImage(image)
			
	def stop(self):
		self.running = False

class NwThread(threading.Thread):
	def __init__(self, nm):
		threading.Thread.__init__(self)
		self.nm = nm
		self.currImage = ''
		
	def run(self):
		self.running = True
		while True == self.running:
			image = nm.receive(921600)
			self.currImage = image
	
	def stop(self):
		self.running = False
		
	def getCurrImage(self):
		return self.currImage

if __name__ == "__main__":
	print("PiVisionGuiMain called")
	nm = PiVisionNwM(3077)
	nm.connect("192.168.1.76")
	resolution = (480, 640)
	gui = PiVisionGui(resolution)
	nwThread = NwThread(nm)
	nwThread.start()
	guiThread = GuiThread(nwThread, gui)
	guiThread.start()
	gui.mainLoop()
	nwThread.stop()
	guiThread.stop()
