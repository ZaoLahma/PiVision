import socket
import threading
import PiVisionConstants

class PiVisionServer(threading.Thread):
	def __init__(self, portNo):
		print("PiVisionServer::__init__ called. portNo: " + str(portNo))
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
		
	def waitForConnection(self):
		self.start()
		
	def send(self, data):
		for connection in self.connections:
			try:
				connection[0].sendall(data)
			except Exception as e:
				print("Disconnecting connection due to: " + str(e))
				self.connections.remove(connection)

	def stop(self):
		self.running = False
		
class PiVisionClient(threading.Thread):
	def __init__(self, imageSize):
		print("PiVisionClient::__init__ called")
		threading.Thread.__init__(self)
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.running = False
		self.data = None
		self.imageSize = imageSize
		
	def connect(self, address, portNo):
		loopback = "127.0.0.1"
		try:
			self.socket = socket.create_connection((loopback, portNo), 0.2)
		except socket.timeout:
			self.socket.connect((address, portNo))
		except socket.error:
			self.socket.connect((address, portNo))
		except:
			raise
		else:
			print("Connection established to PiVision server")
	
	def getData(self):
		return self.data
		
	def receive(self, numBytes):
		data = ''
		while len(data) < numBytes:
			try:
				packet = self.socket.recv(numBytes - len(data))
				if not packet:
					return None
				data += packet
			except socket.timeout:
				pass
		return data	
	
	def run(self):
		print("PiVisionClient::run called")
		self.running = True
		while True == self.running:
			self.data = self.receive(self.imageSize)
			
	def stop(self):
		self.running = False
