package pivision.java.gui;
import java.net.MulticastSocket;
import java.net.Socket;
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.UnknownHostException;
import java.net.SocketTimeoutException;
import java.io.IOException;

public class PiVisClient
{
  private enum PiVisClientState
  {
    INIT_SERVICE_DISCOVERY_SOCKET,
    FIND_SERVICE,
    CONNECT_SERVICE
  }

  private PiVisClient.PiVisClientState state = PiVisClient.PiVisClientState.INIT_SERVICE_DISCOVERY_SOCKET;
  private int serviceNo = 0;
  private final String multicastGroup = "224.1.1.1";
  private MulticastSocket serviceDiscoverySocket = null;
  private String serviceAddress = null;
  private Socket serviceSocket = null;

  public PiVisClient(final int service)
  {
    state = PiVisClient.PiVisClientState.INIT_SERVICE_DISCOVERY_SOCKET;
    serviceNo = service;
  }

  private void initServiceDiscoverySocket()
  {
    try
    {
      serviceDiscoverySocket = new MulticastSocket(serviceNo);
      final int oneSecond = 1 * 1000;
      serviceDiscoverySocket.setSoTimeout(oneSecond);    
      state = PiVisClient.PiVisClientState.FIND_SERVICE;
    }
    catch(IOException exception)
    {
      System.out.println("IOException caught in initServiceDiscoverySocket");
    }
  }

  private void findService()
  {
    final String serviceDiscoveryHeader = "WHERE_IS_";
    final String serviceDiscoveryPayload = serviceDiscoveryHeader + Integer.toString(serviceNo);
    final byte[] bytesToSend = serviceDiscoveryPayload.getBytes();
    try
    {
      final InetAddress group = InetAddress.getByName(multicastGroup);   
      final DatagramPacket packetToSend = new DatagramPacket(bytesToSend, bytesToSend.length, group, serviceNo);
      System.out.println("length:" + Integer.toString(bytesToSend.length));      
      serviceDiscoverySocket.send(packetToSend);
      final int addressLength = 13;
      DatagramPacket packetToReceive = new DatagramPacket(new byte[addressLength], addressLength);
      serviceDiscoverySocket.receive(packetToReceive);
      serviceAddress = new String(packetToReceive.getData(), 0, packetToReceive.getLength());
      System.out.println(serviceAddress);
      serviceDiscoverySocket.close();
      state = PiVisClient.PiVisClientState.CONNECT_SERVICE;
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

    serviceDiscoverySocket = null;
  }

  private void connectService()
  {
    System.out.println("Attempting to connect to: " + Integer.toString(serviceNo) + " at " + serviceAddress);
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

      default:
        break;
    }
  }
}