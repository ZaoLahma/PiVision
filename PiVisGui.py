#!/usr/bin/python

import tkinter
from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient
import PiVisConstants
import threading
from tkinter import messagebox
import sys

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
        self.join()

class PiVisGui:
    def __init__(self, guiScheduler, client, mode):
        self.scheduler = guiScheduler
        self.frameNo = 0
        self.mode = mode
        self.client = client
        self.resolution = PiVisConstants.IMAGE_RESOLUTION
        self.window = tkinter.Tk()
        self.window.protocol("WM_DELETE_WINDOW", self.onClose)
        self.canvas = tkinter.Canvas(self.window, width=self.resolution[0], height=self.resolution[1], bg="#000000")
        self.canvas.pack()
        self.image = tkinter.PhotoImage(width=self.resolution[0], height=self.resolution[1])
        self.canvas.create_image((self.resolution[0]/2, self.resolution[1]/2), image=self.image, state="normal")
        self.hexImage = [None] * self.resolution[1]
        self.hexRow = [None] * (self.resolution[0] + 2)

        guiScheduler.registerRunnable(self.run)

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
        self.scheduler.stop()
        self.window.destroy()

    def showGrayScaleImage(self, image):
        x = 0
        y = 0
        self.hexRow[x] = '{'

        for byte in image:
            self.hexRow[x + 1] = ("#%02x%02x%02x " % (byte, byte, byte))
            x += 1
            if x == self.resolution[0]:
                self.hexRow[x + 1] = '}'
                #print(self.hexRow)
                self.hexImage[y] = ''.join(self.hexRow)
                x = 0
                self.hexRow[x] = ' {'
                y += 1
        self.image.put(''.join(self.hexImage), to=(0, 0, self.resolution[0], self.resolution[1]))

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

    def tkInterMain(self):
        self.window.mainloop()

if __name__ == "__main__":
    guiScheduler = PiVisScheduler()
    clientScheduler = PiVisScheduler()
    guiSchedThread = PiVisGuiSchedThread(guiScheduler)
    clientSchedThread = PiVisGuiSchedThread(clientScheduler)
    client = None
    client = PiVisClient(clientScheduler,
                         PiVisConstants.IMAGE_DATA_SERVICE,
                         PiVisConstants.GRAYSCALE_IMAGE_BYTE_SIZE)
    gui = PiVisGui(guiScheduler, client, "gray")
    guiSchedThread.start()
    clientSchedThread.start()
    gui.tkInterMain()
    guiSchedThread.stop()
