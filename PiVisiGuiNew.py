from tkinter import *
import threading
import PiVisConstants
from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient

GRAY_SCALE_IMAGE_TYPE = 0
COLOR_IMAGE_TYPE      = 1

class PiVisGuiSchedThread(threading.Thread):
    def __init__(self, guiScheduler):
        threading.Thread.__init__(self)
        self.scheduler = guiScheduler

    def run(self):
        self.scheduler.run()

    def stop(self):
        self.scheduler.stop()

class PiVisGui():
    def __init__(self, guiScheduler, clientScheduler, client):
        self.guiScheduler = guiScheduler
        self.clientScheduler = clientScheduler
        self.client = client
        self.frameNo = 0

        self.root = Tk()
        self.frame = Frame(self.root)
        self.frame.pack()

        self.bottomframe = Frame(self.root)
        self.bottomframe.pack( side = BOTTOM )

        self.resolution = PiVisConstants.IMAGE_RESOLUTION
        self.canvas = Canvas(self.frame, width=self.resolution[0], height=self.resolution[1], bg="#000000")
        self.canvas.pack( side = TOP )
        self.image = PhotoImage(width=self.resolution[0], height=self.resolution[1])
        self.canvas.create_image((self.resolution[0]/2, self.resolution[1]/2), image=self.image, state="normal")

        self.blackbutton = Button(self.bottomframe, text="Black", fg="black")
        self.blackbutton.pack( side = BOTTOM)

        self.root.protocol("WM_DELETE_WINDOW", self.onClose)

        guiScheduler.registerRunnable(self.run)

    def showGrayScaleImage(self, image):
        x = 0
        y = 0
        hexImage = []
        hexRow = []
        hexRow.append('{')

        for byte in image:
            hexRow.append("#%02x%02x%02x " % (byte, byte, byte))
            x += 1
            if x == self.resolution[0]:
                hexRow.append('}')
                hexImage.append(''.join(hexRow))
                hexRow = []
                hexRow.append(' {')
                x = 0
                y += 1
        self.image.put(''.join(hexImage), to=(0, 0, self.resolution[0], self.resolution[1]))

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
                R = byte
            elif 1 == byteOffset:
                G = byte
            elif 2 == byteOffset:
                B = byte

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

    def run(self):
        image = self.client.getData()
        if len(image) > 1:
            if image[0] == GRAY_SCALE_IMAGE_TYPE:
                self.showGrayScaleImage(image[1:len(image)])
            elif image[0] == COLOR_IMAGE_TYPE:
                self.showImage(image[1:len(image)])
            ackData = bytearray();
            ackData.extend([self.frameNo])
            self.client.send(ackData)
            self.frameNo += 1
            if self.frameNo > 255:
                self.frameNo = 0

    def onClose(self):
        self.guiScheduler.stop()
        self.clientScheduler.stop()
        self.root.destroy()

    def mainLoop(self):
        self.root.mainloop()

if __name__ == "__main__":
    guiScheduler = PiVisScheduler()
    clientScheduler = PiVisScheduler()
    guiSchedThread = PiVisGuiSchedThread(guiScheduler)
    clientSchedThread = PiVisGuiSchedThread(clientScheduler)
    client = PiVisClient(clientScheduler,
                         PiVisConstants.IMAGE_DATA_SERVICE,
                         PiVisConstants.GRAYSCALE_IMAGE_BYTE_SIZE)
    gui = PiVisGui(guiScheduler, clientScheduler, client)
    guiSchedThread.start()
    clientSchedThread.start()
    gui.mainLoop()
