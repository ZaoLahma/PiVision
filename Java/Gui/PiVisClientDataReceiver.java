package gui;

import gui.PiVisClient.PiVisClientState;

public abstract interface PiVisClientDataReceiver
{
  abstract void update(final byte[] data);
  abstract void updateState(final PiVisClientState state);
}