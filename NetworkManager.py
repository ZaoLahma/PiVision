import socket

class NetworkManager:
	def __init__(self, portNo):
		print("NetworkManager::__init__ called. portNo: " + str(portNo))
		self.portNo = portNo
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.socket.bind(('', self.portNo))
		self.socket.listen(1)
		
		
	def waitForConnection(self):
		self.connection, self.address = self.socket.accept()
		print("NetworkManager::waitForConnection: Connected to by " + str(self.address))
		while 1:
			data = self.connection.recv(1024)
			print(str(data))
