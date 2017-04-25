import Tkinter
import time
import threading
from PiVisionNwM import PiVisionClient
from PiVisionPpmImageCreator import PiVisionPpmImageCreator

class PiVisionGuiThread(threading.Thread):
	def __init__(self, nwThread, gui):
		threading.Thread.__init__(self)
		self.nwThread = nwThread
		self.gui = gui
		
	def run(self):
		self.running = True
		while True == self.running:
			image = self.nwThread.getData()
			if None != image:
				self.gui.showImage(image)
			
	def stop(self):
		self.running = False

class PiVisionGui:
	def __init__(self, resolution):
		self.resolution = resolution
		self.window = Tkinter.Tk()
		self.canvas = Tkinter.Canvas(self.window, width=self.resolution[1], height=self.resolution[0], bg="#000000")
		self.canvas.pack()
		self.image = Tkinter.PhotoImage(width=self.resolution[1], height=self.resolution[0])
		self.canvas.create_image((self.resolution[1]/2, self.resolution[0]/2), image=self.image, state="normal")
			
	# Prints an image in raw RGB format. Slow as heckish.
	def showImage(self, image):
		print("PiVisionGui::showImage called")
		imageCreator = PiVisionPpmImageCreator()
		image = imageCreator.createPpmImage(image)
		self.image.put(image, to=(0, 0, self.resolution[1], self.resolution[0]))


	def mainLoop(self):
		self.window.mainloop()

if __name__ == "__main__":
	print("PiVisionGuiMain called")
	nm = PiVisionClient()
	nm.connect("192.168.1.200", 3077)
	nm.start()	
	resolution = (480, 640)
	gui = PiVisionGui(resolution)
	guiThread = PiVisionGuiThread(nm, gui)
	guiThread.start()
	gui.mainLoop()
	guiThread.stop()
	nm.stop()