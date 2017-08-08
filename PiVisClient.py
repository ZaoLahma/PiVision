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
        self.receiveBuf = []
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
        
    def receive(self):
        headerSize = 9 #1 byte image type, 4 bytes image size, 2 bytes xSize, 2 bytes ySize
        header = []
        while len(header) < headerSize:
            try:
                packet = self.serverSocket.recv(headerSize - len(header))
                if not packet:
                    continue
                header += packet
            except socket.timeout:
                pass 

        imageSize = bytearray(header[1:5])
        imageSize = struct.unpack("<L", imageSize)[0]

        imageType = header[0:1]
        
        while len(self.receiveBuf) < imageSize:
            try:
                packet = self.serverSocket.recv(imageSize - len(self.receiveBuf))
                if not packet:
                    continue
                self.receiveBuf += packet
            except socket.timeout:
                pass
            
        tmpBuf = []
        tmpBuf += imageType
        tmpBuf += self.receiveBuf       
        self.data = tmpBuf
        self.receiveBuf = []
        
    def send(self, data):
        self.serverSocket.sendall(data)
    
    def run(self):
        self.states[self.stateIndex]()