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
        self.portNo = portNo
        self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connections = []
        self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.serverSocket.bind(('', self.portNo)) 
        self.serviceListenerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.serviceListenerSocket.bind(('224.1.1.1', PiVisConstants.DISCOVER_SERVICE))
        self.host = getOwnIp()
        self.serviceListenerSocket.setsockopt(socket.SOL_IP, socket.IP_MULTICAST_IF, 
											  socket.inet_aton(self.host))
        self.serviceListenerSocket.setsockopt(socket.SOL_IP, 
                                              socket.IP_ADD_MEMBERSHIP, 
                                              socket.inet_aton('224.1.1.1') + 
                                              socket.inet_aton(self.host))
        scheduler.registerRunnable(self.run)
        
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
        PiVisConstants.SERVICE_DISCOVER_REQUEST_HEADER
        self.serviceListenerSocket.settimeout(0.001)
        try:
            request = self.serviceListenerSocket.recvfrom(len(PiVisConstants.SERVICE_DISCOVER_REQUEST_HEADER))
        except socket.timeout:
            pass
        except:
            raise
        else:
            print("PiVisServer::__handleServiceDiscoveryRequests__: New service request received from " + str(request[1]))
            response = bytearray()
            response.extend(map(ord, self.host))
            responseSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            responseSocket.sendto(response, request[1])

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
