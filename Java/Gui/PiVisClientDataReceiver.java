package pivision.java.gui;

import pivision.java.gui.PiVisClient.PiVisClientState;

public abstract interface PiVisClientDataReceiver
{
  abstract void update(final byte[] data);
  abstract void updateState(final PiVisClientState state);
}