package pivision.java.gui;

public abstract interface PiVisClientDataReceiver
{
  abstract void update(final byte[] data);
}