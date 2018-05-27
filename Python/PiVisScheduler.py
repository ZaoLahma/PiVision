#!/usr/bin/python

class PiVisScheduler:
    def __init__(self):
        self.runnables = []
        self.stoppers = []
        self.running = False

    def registerRunnable(self, runnable):
        print("Runnable added to scheduler: " + str(runnable))
        self.runnables.append(runnable)

    def registerStopper(self, stopper):
        self.stoppers.append(stopper)

    def run(self):
        self.running = True
        while True == self.running:
            for runnable in self.runnables:
                runnable()

    def stop(self):
        self.running = False
        for stopper in self.stoppers:
            stopper()
