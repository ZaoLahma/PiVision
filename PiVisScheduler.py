#!/usr/bin/python

class PiVisScheduler:
    def __init__(self):
        self.runnables = []
        self.running = False

    def registerRunnable(self, runnable):
        print("Runnable added to scheduler: " + str(runnable))
        self.runnables.append(runnable)

    def run(self):
        self.running = True
        while True == self.running:
            for runnable in self.runnables:
                runnable()
