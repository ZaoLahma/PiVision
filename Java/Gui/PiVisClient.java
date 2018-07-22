package pivision.java.gui;
import java.net.MulticastSocket;
import java.net.Socket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.DatagramPacket;
import java.net.UnknownHostException;
import java.net.SocketTimeoutException;
import java.io.IOException;
import java.io.EOFException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import pivision.java.gui.PiVisClientDataReceiver;

public class PiVisClient
{
  private enum PiVisClientState
  {
    INIT_SERVICE_DISCOVERY_SOCKET,
    FIND_SERVICE,
    CONNECT_SERVICE,
    CONNECTED
  }

  private PiVisClient.PiVisClientState state = PiVisClient.PiVisClientState.INIT_SERVICE_DISCOVERY_SOCKET;
  private int serviceNo = 0;
  private final String multicastGroup = "224.1.1.1";
  private MulticastSocket serviceDiscoverySocket = null;
  private String serviceAddress = null;
  private Socket serviceSocket = null;
  private PiVisClientDataReceiver dataReceiver = null;
  private boolean active;

  public PiVisClient(final int service)
  {
    state = PiVisClient.PiVisClientState.INIT_SERVICE_DISCOVERY_SOCKET;
    serviceNo = service;
    active = true;
  }

  private void initServiceDiscoverySocket()
  {
    try
    {
      System.out.println("initServiceDiscoverySocket");
      serviceDiscoverySocket = new MulticastSocket(serviceNo);
      final InetAddress group = InetAddress.getByName(multicastGroup);
      //serviceDiscoverySocket.joinGroup(group);
      serviceDiscoverySocket.setLoopbackMode(true);
      System.out.println("Socket created");
      final int twoSeconds = 2 * 1000;
      serviceDiscoverySocket.setSoTimeout(twoSeconds);    
      state = PiVisClient.PiVisClientState.FIND_SERVICE;
    }
    catch(IOException exception)
    {
      System.out.println("IOException caught in initServiceDiscoverySocket");
    }
  }

  private void findService()
  {
    System.out.println("findService");
    final String serviceDiscoveryHeader = "WHERE_IS_";
    final String serviceDiscoveryPayload = serviceDiscoveryHeader + Integer.toString(serviceNo);
    System.out.println("serviceDiscoveryPayload: " + serviceDiscoveryPayload);
    final byte[] bytesToSend = serviceDiscoveryPayload.getBytes();
    try
    {
      final InetAddress group = InetAddress.getByName(multicastGroup);
      DatagramPacket packetToSend = new DatagramPacket(bytesToSend, bytesToSend.length, group, serviceNo);
      System.out.println("ServiceDiscovery sending " + Integer.toString(bytesToSend.length) + " bytes");      
      serviceDiscoverySocket.send(packetToSend);
      final int addressLength = 16;
      byte[] payload = new byte[addressLength];
      DatagramPacket packetToReceive = new DatagramPacket(payload, payload.length);
      serviceDiscoverySocket.receive(packetToReceive);
      serviceAddress = new String(payload);
      System.out.println(serviceAddress);
      if(!serviceAddress.contains(serviceDiscoveryPayload))
      {
        serviceDiscoverySocket.close();
        serviceDiscoverySocket = null;
        System.out.println("serviceAddress: " + serviceAddress + " serviceDiscoveryPayload: " + serviceDiscoveryPayload);
        state = PiVisClient.PiVisClientState.CONNECT_SERVICE;
      }
      else
      {
        System.out.println("SAME!!!");
      }
    }
    catch(UnknownHostException exception)
    {
      System.out.println("UnknownHostException caught in findService");
    }
    catch(SocketTimeoutException exception)
    {
      System.out.println("Socket timeout");
    }
    catch(IOException exception)
    {
      System.out.println(exception);
    }
  }

  private void connectService()
  {
    System.out.println("Attempting to connect to: " + Integer.toString(serviceNo) + " at " + serviceAddress);

    try
    {
      serviceSocket = new Socket();
      final int oneSecond = 1 * 1000;
      serviceSocket.setSoTimeout(oneSecond);        
      serviceSocket.connect(new InetSocketAddress(serviceAddress, serviceNo));
      state = PiVisClient.PiVisClientState.CONNECTED;
      System.out.println("PiVisClient connected to server");
    }
    catch(UnknownHostException exception)
    {
      System.out.println(exception);
    }
    catch(SocketTimeoutException exception)
    {
      System.out.println(exception);
    }
    catch(IOException exception)
    {
      System.out.println(exception);
    }
  }

  private void connected()
  {
    int numBytesToFetch = 4;
    try
    {
      DataInputStream networkInput = new DataInputStream(serviceSocket.getInputStream());
      int numBytesToReceive = Integer.reverseBytes(networkInput.readInt());
      //System.out.println(numBytesToReceive);
      byte[] payload = new byte[numBytesToReceive];
      networkInput.readFully(payload, 0, payload.length);
      DataOutputStream networkOutput = new DataOutputStream(serviceSocket.getOutputStream());
      final int ackMsgHeader = 4;
      networkOutput.writeInt(Integer.reverseBytes(ackMsgHeader));
      final int ackMsgPayload = 0xDEADBEEF;
      networkOutput.writeInt(Integer.reverseBytes(ackMsgPayload));
      //System.out.println("Sent ackMsg");
      if(4 != numBytesToReceive)
      {
        if(null != dataReceiver)
        {
          dataReceiver.update(payload);
        }
      }
    }
    catch(EOFException exception)
    {
      System.out.println(exception);
      System.exit(0);
    }
    catch(IOException exception)
    {
      System.out.println(exception);
    }
  }

  public void setDataReceiver(PiVisClientDataReceiver dataReceiver)
  {
    this.dataReceiver = dataReceiver;
  }

  public boolean active()
  {
    return active;
  }

  public void stop()
  {
    active = false;
  }

  public void run()
  {
    switch(state)
    {
      case INIT_SERVICE_DISCOVERY_SOCKET:
        initServiceDiscoverySocket();
        break;
      case FIND_SERVICE:
        findService();
        break;
      case CONNECT_SERVICE:
        connectService();
        break;
      case CONNECTED:
        connected();
        break;
      default:
        break;
    }
  }
}