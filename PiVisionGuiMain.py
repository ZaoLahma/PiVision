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
		self.image = ''
		
	def run(self):
		self.running = True
		while True == self.running:
			self.image = nm.receive(921600)
	
	def stop(self):
		self.running = False
		
	def getCurrImage(self):
		return self.image

if __name__ == "__main__":
	print("PiVisionGuiMain called")
	nm = PiVisionNwM(3077)
	nm.connect("127.0.0.1")
	resolution = (480, 640)
	gui = PiVisionGui(resolution)
	nwThread = NwThread(nm)
	nwThread.start()
	guiThread = GuiThread(nwThread, gui)
	guiThread.start()
	gui.mainLoop()
	nwThread.stop()
	guiThread.stop()
