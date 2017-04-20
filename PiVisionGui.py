import Tkinter
import math
import binascii

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
		hexImage = "#"
		byteOffset = 0
		x = 0
		y = 0
		byteCounter = 0
		for byte in image:
			hexImage += "%02x" % ord(byte)
			byteOffset += 1
			byteCounter += 1
			if 3 == byteOffset:
				#print("x: " + str(x) + " y: " + str(y))
				byteOffset = 0
				#print(hexImage)
				self.image.put(hexImage, (x, y))
				hexImage = "#"
				x += 1
				if x == self.resolution[1]:
					x = 0
					y += 1
		print("byteCounter: " + str(byteCounter))


	def mainLoop(self):
		self.window.mainloop()
