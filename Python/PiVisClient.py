#!/usr/bin/python

import socket
import struct
import PiVisConstants

class PiVisClient:
    def __init__(self, scheduler, portNo, imageSize):
        self.serviceNo = -1
        self.portNo = portNo
        self.serviceNo = PiVisConstants.DISCOVER_IMAGE_DATA_SERVICE
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)     
        self.serviceDiscoverySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.serviceDiscoverySocket.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 2)
        self.stateIndex = 0
        self.states = [self.findService, self.connect, self.connected]
        self.piVisServerAddress = ""
        self.imageSize = imageSize
        self.data = []
        scheduler.registerRunnable(self.run)
    
    def findService(self):
        data = bytearray()
        data.extend(map(ord, PiVisConstants.SERVICE_DISCOVER_REQUEST_HEADER + str(self.portNo)))
        self.serviceDiscoverySocket.sendto(data,  ("224.1.1.1", self.serviceNo))
        self.serviceDiscoverySocket.settimeout(1)
        try:
            data = self.serviceDiscoverySocket.recv(13)
        except socket.timeout:
            pass
        except:
            raise
        else:
            self.serviceDiscoverySocket.close()
            self.piVisServerAddress = str(data, 'utf-8')
            print("PiVisServer service found at " + str(self.piVisServerAddress))
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
        headerSize = 9 #1 byte image type, 4 bytes image size, 2 bytes xSize, 2 bytes ySize

        header = self.__receiveInternal(headerSize)

        imageSize = bytearray(header[1:5])
        imageSize = struct.unpack("<L", imageSize)[0]

        imageType = header[0:1]
        
        receiveBuf = self.__receiveInternal(imageSize)
        
        tmpBuf = []
        tmpBuf += imageType
        tmpBuf += receiveBuf    
        self.data = tmpBuf
        
    def send(self, data):
        self.serverSocket.sendall(data)
    
    def run(self):
        self.states[self.stateIndex]()