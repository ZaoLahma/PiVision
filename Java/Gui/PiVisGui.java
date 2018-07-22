package pivision.java.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import pivision.java.gui.PiVisClient;
import pivision.java.gui.PiVisClientDataReceiver;

public class PiVisGui implements ActionListener, PiVisClientDataReceiver
{

  private GuiDrawPanel drawPanel;
  private PiVisClient client = null;

  PiVisGui(PiVisClient client)
  {
    this.client = client;
  }

  public void run() 
  {
    drawPanel = new GuiDrawPanel(640, 480);

    JFrame frame = new JFrame();
    frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

    frame.addWindowListener(new WindowAdapter()
    {
      public void windowClosing(WindowEvent e)
      {
        System.out.println("Window close");
        client.stop();
      }
    });   

    JButton button = new JButton("Button that does nothing");
    button.addActionListener( this );

    frame.add(drawPanel, BorderLayout.CENTER);
    frame.add(button, BorderLayout.PAGE_END);
    frame.pack();
    frame.setLocationByPlatform(true);
    frame.setVisible(true);

    client.setDataReceiver(this);
    Runnable clientRunnable = new Runnable()
    {
      public void run()
      {
        while(client.active())
        {
          client.run();
        }
      }
    };
    Thread clientThread = new Thread(clientRunnable);
    clientThread.start();
  }

  public void actionPerformed(ActionEvent event) 
  {
    System.out.println("Button pressed!");
  }

  public void update(final byte[] data)
  {
    drawPanel.drawImage(data);
    drawPanel.updateKBPerSec(client.getKBytesPerSec());
  }
}

class Main
{
  public static void main(String[] args) 
  {
    PiVisClient client = new PiVisClient(3380);
    PiVisGui gui = new PiVisGui(client);   
    Runnable guiRunnable = new Runnable() 
    {
      @Override
      public void run() 
      {
          gui.run();
      }
    };
    EventQueue.invokeLater(guiRunnable);
  }
}

class GuiDrawPanel extends JPanel {

    private int width = 300;
    private int height = 300;

    private int fps = 0;
    private int highestFps = 0;
    private int ticks = 0;
    private long prevTime;

    private int kBPerSec = 0;
    private int highestKBPerSec = 0;    

    private byte[] imageData;

    private Color getMeasurementColor(final int value, final int highestVal)
    {
      int red = 0;
      int green = 0;
      int blue = 0;

      final int minVal = 120;
      final int maxVal = 255;
      
      float percentage = 0;

      if(maxVal > 0)
      {
        percentage = (float)value / (float)highestVal;
      }

      //System.out.println("percentage: " + Float.toString(percentage));

      green = minVal + (int)(((maxVal - minVal) * percentage) + 0.5);
      red = minVal + (int)(((maxVal - minVal) * (1 - percentage)) + 0.5);

      //System.out.println("RGB: " + Integer.toString(red) + " " + Integer.toString(green) + " " + Integer.toString(blue));

      return new Color(red, green, blue);
    }

    GuiDrawPanel(final int width, final int height)
    {
      this.width = width;
      this.height = height;

      this.imageData = new byte[width * height];

      for(int x = 0; x < imageData.length; ++x)
      {
        imageData[x] = (byte)0x0;
      }

      prevTime = System.currentTimeMillis();
    }

    public Dimension getPreferredSize () 
    {
        return new Dimension (width, height + 50);
    }

    public void updateKBPerSec(final int kBPerSec)
    {
      this.kBPerSec = kBPerSec;

      if(this.kBPerSec > highestKBPerSec)
      {
        highestKBPerSec = this.kBPerSec;
      }
    }

    public void drawImage(byte[] imageData)
    {
      synchronized(this.imageData)
      {
        this.imageData = imageData;
      }

      repaint();
    }
  
    protected void paintComponent(Graphics g) 
    {
      super.paintComponent(g);

      synchronized(imageData)
      {
        int x = 0;
        int y = 0;
        for(int byteIndex = 0; byteIndex < imageData.length; ++byteIndex)
        {
          final int colorIntensity = Byte.toUnsignedInt(imageData[byteIndex]);
          g.setColor(new Color(colorIntensity, colorIntensity, colorIntensity));
          g.drawLine(x, y, x, y);
          x += 1;
          if(x == width)
          {
            x = 0;
            y += 1;
          }
        }
      }

      long time = System.currentTimeMillis();
      int timeElapsed = (int)(time - prevTime);
      if(timeElapsed > 1000)
      {
        fps = timeElapsed * ticks / timeElapsed;

        if(fps > highestFps)
        {
          highestFps = fps;
        }

        ticks = 0;
        prevTime = time;
      }

      Color fpsColor = getMeasurementColor(fps, highestFps);
      g.setColor(fpsColor);
      g.drawString("Current FPS: " + Integer.toString(fps), 10, 20);
      Color kBPerSecColor = getMeasurementColor(kBPerSec, highestKBPerSec);
      g.setColor(kBPerSecColor);
      g.drawString("kB/s: " + Integer.toString(kBPerSec), 10, 35);
      ticks = ticks + 1;

      //System.out.println("timeElapsed: " + Long.toString(timeElapsed) + ", fps: " + Integer.toString(fps));
    }
}