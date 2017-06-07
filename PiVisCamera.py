#!/usr/bin/python

class PiVisCamera:
    def __init__(self, scheduler, server):
        self.server = server        
        scheduler.registerRunnable(self.run)

    def run(self):
        data = [127, 0, 0, 1]
        self.server.send(bytearray(data))