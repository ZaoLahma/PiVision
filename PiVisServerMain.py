#!/usr/bin/python
from PiVisScheduler import PiVisScheduler
from PiVisCamera import PiVisCamera
from PiVisServer import PiVisServer
import PiVisConstants

if __name__ == "__main__":
    print("PiVisServer starting")
    scheduler = PiVisScheduler()
    server = PiVisServer(scheduler, PiVisConstants.COLOR_SERVICE, PiVisConstants.IMAGE_RESOLUTION)    
    camera = PiVisCamera(scheduler, server)
    scheduler.run()