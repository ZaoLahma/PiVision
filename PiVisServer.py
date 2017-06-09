#!/usr/bin/python

import socket
import PiVisConstants

def getOwnIp():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(('5.255.255.255', 1))
    IP = s.getsockname()[0]
    s.close()
    return IP

class PiVisServer:
    def __init__(self, scheduler, portNo):
        self.serviceNo = -1
        self.portNo = portNo
        if self.portNo == PiVisConstants.COLOR_SERVICE:
            self.serviceNo = PiVisConstants.DISCOVER_COLOR_SERVICE
        elif self.portNo == PiVisConstants.GRAYSCALE_SERVICE:
            self.serviceNo = PiVisConstants.DISCOVER_GRAY_SERVICE
        self.host = getOwnIp()
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connections = []
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.serverSocket.bind(('', self.portNo)) 
        self.__setUpServiceListener__()
        scheduler.registerRunnable(self.run)
    
    def __setUpServiceListener__(self):
        self.serviceListenerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.serviceListenerSocket.bind(('224.1.1.1', self.serviceNo))
        self.serviceListenerSocket.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_IF, 
											  socket.inet_aton(self.host))
        self.serviceListenerSocket.setsockopt(socket.SOL_IP, 
                                              socket.IP_ADD_MEMBERSHIP, 
                                              socket.inet_aton('224.1.1.1') + 
                                              socket.inet_aton(self.host))  
    def __handleNewConnections__(self):
        try:
            self.serverSocket.settimeout(0.001)
            self.serverSocket.listen(1)
            (connection, address) = self.serverSocket.accept()
        except socket.timeout:
            pass
        except:
            raise
        else:
            print("PiVisionNwM::waitForConnection: Connected to by " + str(address))
            self.connections.append((connection, address))
    
    def __handleServiceDiscoveryRequests__(self):
        self.serviceListenerSocket.settimeout(0.001)
        try:
            request = self.serviceListenerSocket.recvfrom(4096)
        except socket.timeout:
            pass
        except:
            raise
        else:
            print("PiVisServer::__handleServiceDiscoveryRequests__: New service request for " + str(request[0]) + " received from " + str(request[1]))
            reqString = str(request[0], 'utf-8')
            if reqString.startswith(PiVisConstants.SERVICE_DISCOVER_REQUEST_HEADER):
                splitReqString = str.split(reqString, '_')
                requestedPortNo = splitReqString[2]
                if requestedPortNo.endswith("\x00"):
                    requestedPortNo = requestedPortNo[:-1]
                if int(requestedPortNo) == self.portNo:
                    response = bytearray()
                    response.extend(map(ord, self.host))
                    responseSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    responseSocket.sendto(response, request[1])
                    responseSocket.close()
                else:
                    print("Discarded request. Wrong service")
            else:
                print("Discared request. Wrong header")   
            self.serviceListenerSocket.close()
            self.__setUpServiceListener__()

    def run(self):
        self.__handleNewConnections__()
        self.__handleServiceDiscoveryRequests__()
            
    def send(self, data):
        for connection in self.connections:
            try:
                connection[0].sendall(data)
            except Exception as e:
                print("Disconnecting connection due to: " + str(e))
                self.connections.remove(connection)
