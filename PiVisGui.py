#!/usr/bin/python

import tkinter
from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient
import PiVisConstants
import threading
from tkinter import messagebox
import sys

class PiVisGuiSchedThread(threading.Thread):
    def __init__(self, scheduler):
        threading.Thread.__init__(self)
        self.scheduler = scheduler
        
    def run(self):
        self.scheduler.run()
        
    def stop(self):
        self.scheduler.stop()
        self.join()

class PiVisGui:
    def __init__(self, scheduler, client, mode):
        self.scheduler = scheduler
        self.mode = mode
        self.client = client
        self.resolution = PiVisConstants.IMAGE_RESOLUTION
        self.window = tkinter.Tk()
        self.window.protocol("WM_DELETE_WINDOW", self.onClose)
        self.canvas = tkinter.Canvas(self.window, width=self.resolution[0], height=self.resolution[1], bg="#000000")
        self.canvas.pack()
        self.image = tkinter.PhotoImage(width=self.resolution[0], height=self.resolution[1])
        self.canvas.create_image((self.resolution[0]/2, self.resolution[1]/2), image=self.image, state="normal")        
        scheduler.registerRunnable(self.run)
        
    def run(self):
        image = self.client.getData()
        if "color" == mode:
            self.showImage(image)
        elif "gray" == mode:
            self.showGrayScaleImage(image)
        
    def onClose(self):
        self.scheduler.stop()
        self.window.destroy()
    
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
     
    def tkInterMain(self):
        self.window.mainloop()
        
if __name__ == "__main__":
    scheduler = PiVisScheduler()
    schedThread = PiVisGuiSchedThread(scheduler)
    client = None
    mode = sys.argv[1]
    if mode == "color":
        client = PiVisClient(scheduler, 
                             PiVisConstants.COLOR_SERVICE, 
                             PiVisConstants.IMAGE_BYTE_SIZE)
    elif mode == "gray":
        client = PiVisClient(scheduler, 
                             PiVisConstants.GRAYSCALE_SERVICE, 
                             PiVisConstants.GRAYSCALE_IMAGE_BYTE_SIZE)        
    gui = PiVisGui(scheduler, client, mode)
    schedThread.start()
    gui.tkInterMain()
    schedThread.stop()