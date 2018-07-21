package pivision.java.gui;

import pivision.java.gui.PiVisGui;

public class PiVisGuiEntry
{
  public static void main(String[] args)
  {
    System.out.println("Main running");

    PiVisGui gui = new PiVisGui();
    final int imageDataService = 3380;
    PiVisClient client = new PiVisClient(imageDataService);


    //while(true)
    {
      client.run();
      client.run();
      client.run();
    }
  }
}