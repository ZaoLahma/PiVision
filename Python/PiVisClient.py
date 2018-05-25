#!/usr/bin/python

import socket
import struct
import PiVisConstants

class PiVisClient:
    def __init__(self, scheduler, portNo):
        self.serviceNo = -1
        self.portNo = portNo
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serviceDiscoverySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.serviceDiscoverySocket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        self.stateIndex = 0
        self.states = [self.findService, self.connect, self.connected]
        self.piVisServerAddress = ""
        self.data = []
        scheduler.registerRunnable(self.run)

    def findService(self):
        data = bytearray()
        address = bytearray()
        data.extend(map(ord, PiVisConstants.SERVICE_DISCOVER_REQUEST_HEADER + str(self.portNo)))
        self.serviceDiscoverySocket.sendto(data,  ("224.1.1.1", self.portNo))
        self.serviceDiscoverySocket.settimeout(1)
        try:
            address = self.serviceDiscoverySocket.recv(13)
        except socket.timeout:
            pass
        except:
            raise
        else:
            self.serviceDiscoverySocket.close()
            self.piVisServerAddress = str(address, 'utf-8').split('\x00')
            if len(self.piVisServerAddress):
                self.piVisServerAddress = self.piVisServerAddress[0]
            print("PiVisServer service found at " + self.piVisServerAddress)
            self.stateIndex += 1

    def connect(self):
        try:
            print("Attempting to connect to " + self.piVisServerAddress + " " + str(self.portNo))
            self.serverSocket.connect((self.piVisServerAddress, self.portNo))
        except:
            raise
        else:
            print("Connection established to PiVision server")
            self.stateIndex = self.stateIndex + 1

    def connected(self):
        self.receive()

    def getData(self):
        retVal = self.data
        return retVal

    def __receiveInternal(self, numBytes):
        data = []
        while len(data) < numBytes:
            try:
                packet = self.serverSocket.recv(numBytes - len(data))
                if not packet:
                    continue
                data += packet
            except socket.timeout:
                pass
        return data

    def receive(self):
        headerSize = 8 # 4 bytes image size, 2 bytes xSize, 2 bytes ySize

        header = self.__receiveInternal(headerSize)

        imageSize = bytearray(header[0:4])
        imageSize = struct.unpack("<L", imageSize)[0]

        receiveBuf = self.__receiveInternal(imageSize)

        print("imageSize: " + str(imageSize))

        tmpBuf = []
        tmpBuf += [PiVisConstants.GRAY_SCALE_IMAGE_TYPE]
        tmpBuf += receiveBuf
        self.data = tmpBuf

    def send(self, data):
        self.serverSocket.sendall(data)

    def run(self):
        self.states[self.stateIndex]()
