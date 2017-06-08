#!/usr/bin/python

class PiVisGrayScaleServer:
    def __init__(self, scheduler):
        scheduler.register(self.run)
        
    def run(self):
        print("PiVisGrayScaleServer run called")