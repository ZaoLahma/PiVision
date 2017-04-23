import Tkinter
import time

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
		startTime = time.time()
		print("PiVisionGui::showImage called. startTime: " + str(startTime))
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
				if x == self.resolution[1]:
					hexRow.append('}')
					hexImage.append(''.join(hexRow))
					hexRow = []
					hexRow.append(' {')
					x = 0
					y += 1
		
		print("Finished creating hexImage in " + str(time.time() - startTime) + "s")
		startTime = time.time()
		self.image.put(''.join(hexImage), to=(0, 0, self.resolution[1], self.resolution[0]))
		print("num bytes handled: " + str(byteCounter) + " in " + str(time.time() - startTime) + "s")


	def mainLoop(self):
		self.window.mainloop()
