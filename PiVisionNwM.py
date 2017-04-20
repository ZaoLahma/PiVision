import socket
import threading

class PiVisionNwM(threading.Thread):
	def __init__(self, portNo):
		print("PiVisionNwM::__init__ called. portNo: " + str(portNo))
		threading.Thread.__init__(self)
		self.portNo = portNo
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.connections = []
		self.running = False
		
	def run(self):
		self.running = True
		self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.socket.bind(('', self.portNo))		
		while True == self.running:
			try:
				self.socket.settimeout(0.2)
				self.socket.listen(1)
				(connection, address) = self.socket.accept()
			except socket.timeout:
				pass
			except:
				raise
			else:
				print("PiVisionNwM::waitForConnection: Connected to by " + str(address))
				self.connections.append((connection, address))
	
	def __waitForConnection__(self):
		self.start()	
		
	def waitForConnection(self):
		self.__waitForConnection__()
		
	def send(self, data):
		for connection in self.connections:
			connection[0].sendall(data)
			
	def connect(self, address):
		self.socket.connect((address, self.portNo))
		
	def receive(self, numBytes):
		data = ''
		while len(data) < numBytes:
			packet = self.socket.recv(numBytes - len(data))
			if not packet:
				return None
			data += packet
			
		return data
		
	def stop(self):
		self.running = False
