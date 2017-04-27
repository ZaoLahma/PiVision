import Tkinter
import time
import threading
from PiVisionNwM import PiVisionClient
import PiVisionConstants

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
		self.canvas = Tkinter.Canvas(self.window, width=self.resolution[0], height=self.resolution[1], bg="#000000")
		self.canvas.pack()
		self.image = Tkinter.PhotoImage(width=self.resolution[0], height=self.resolution[1])
		self.canvas.create_image((self.resolution[0]/2, self.resolution[1]/2), image=self.image, state="normal")
			
	# Prints an image in raw RGB format. Slow as heckish.
	def showImage(self, image):
		byteOffset = 0
		byteCounter = 0
		x = 0
		y = 0
		R = 0
		G = 0
		B = 0
		hexImage = []
		hexRow = []
		hexRow.append('{')
		
		for byte in image:
			byteCounter += 1			
			if 0 == byteOffset:
				R = ord(byte)
			elif 1 == byteOffset:
				G = ord(byte)
			elif 2 == byteOffset:
				B = ord(byte)
			
			byteOffset += 1
			if 3 == byteOffset:
				byteOffset = 0
				hexRow.append("#%02x%02x%02x " % (R, G, B))
				x += 1
				if x == self.resolution[0]:
					hexRow.append('}')
					hexImage.append(''.join(hexRow))
					hexRow = []
					hexRow.append(' {')
					x = 0
					y += 1
		self.image.put(''.join(hexImage), to=(0, 0, self.resolution[0], self.resolution[1]))


	def mainLoop(self):
		self.window.mainloop()

if __name__ == "__main__":
	print("PiVisionGuiMain called")
	nm = PiVisionClient()
	nm.connect("192.168.1.250", 3077)
	nm.start()
	gui = PiVisionGui(PiVisionConstants.IMAGE_RESOLUTION)
	guiThread = PiVisionGuiThread(nm, gui)
	guiThread.start()
	gui.mainLoop()
	guiThread.stop()
	nm.stop()