import pygame
import threading
import PiVisConstants
from PiVisScheduler import PiVisScheduler
from PiVisClient import PiVisClient
import time

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

        self.resolution = PiVisConstants.IMAGE_RESOLUTION

        pygame.init()
        pygame.display.set_mode((self.resolution[0], self.resolution[1]), pygame.HWSURFACE)
        self.surface = pygame.Surface ((self.resolution[0], self.resolution[1]))
        pygame.display.flip()

        guiScheduler.registerRunnable(self.run)

    def showGrayScaleImage(self, image):
        x = 0
        y = 0

        now = time.time()
        ar = pygame.PixelArray(self.surface)
        for byte in image:
            #print("Byte: " + hex(byte))
            ar[x, y] = (byte, byte, byte)
            x += 1
            if x == self.resolution[0]:
                y += 1
                x = 0
        del ar

        screen = pygame.display.get_surface()
        screen.blit (self.surface, (0, 0))
        pygame.display.update()
        pygame.display.flip()

        after = time.time()

        print("Printing image took: " + str(now - after))

    def run(self):
        image = self.client.getData()
        if len(image) > 1:
            if image[0] == PiVisConstants.GRAY_SCALE_IMAGE_TYPE:
                self.showGrayScaleImage(image[1:len(image)])
            elif image[0] == PiVisConstants.COLOR_IMAGE_TYPE:
                self.showImage(image[1:len(image)])
            ackData = bytearray();
            ackData.extend([self.frameNo])
            self.client.send(ackData)
            self.frameNo += 1
            if self.frameNo > 255:
                self.frameNo = 0


    def mainLoop(self):
        print("Mainloop running")
        while 1:
            event = pygame.event.wait ()
            if event.type == pygame.QUIT:
                self.clientScheduler.stop()
                self.guiScheduler.stop()
                raise SystemExit

if __name__ == "__main__":
    guiScheduler = PiVisScheduler()
    clientScheduler = PiVisScheduler()
    guiSchedThread = PiVisGuiSchedThread(guiScheduler)
    clientSchedThread = PiVisGuiSchedThread(clientScheduler)
    client = PiVisClient(clientScheduler,
                         PiVisConstants.IMAGE_DATA_SERVICE)
    gui = PiVisGui(guiScheduler, clientScheduler, client)
    guiSchedThread.start()
    clientSchedThread.start()
    gui.mainLoop()
