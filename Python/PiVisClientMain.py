#!/usr/bin/python

from PiVisClient import PiVisClient
from PiVisScheduler import PiVisScheduler
import PiVisConstants

if __name__ == "__main__":
    scheduler = PiVisScheduler()
    client = PiVisClient(scheduler,
                         PiVisConstants.COLOR_SERVICE, 
                         PiVisConstants.IMAGE_BYTE_SIZE)
    
    scheduler.run()