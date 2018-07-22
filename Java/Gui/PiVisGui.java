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

    JButton button = new JButton( "Change colors" );
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
    drawPanel.setValues();
  }

  public void update(final byte[] data)
  {
    drawPanel.drawImage(data);
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

    private int red;
    private int green;
    private int blue;

    private Color randomColor;

    private byte[] imageData;

    GuiDrawPanel(final int width, final int height)
    {
      this.width = width;
      this.height = height;

      this.imageData = new byte[width * height];

      for(int x = 0; x < imageData.length; ++x)
      {
        imageData[x] = (byte)0xAB;
      }
    }

    /*
     * Make this one customary habbit,
     * of overriding this method, when
     * you extends a JPanel/JComponent,
     * to define it's Preferred Size.
     * Now in this case we want it to be 
     * as big as the Image itself.
     */
    @Override
    public Dimension getPreferredSize () 
    {
        return new Dimension (width, height + 50);
    }

    public void setValues () 
    {
      red = (int) (Math.random() * 255);
      green = (int) (Math.random() * 255);
      blue = (int) (Math.random() * 255);

      randomColor = new Color(red, green, blue);

      repaint();
    }

    public void drawImage(byte[] imageData)
    {
      this.imageData = imageData;

      repaint();
    }
  
    protected void paintComponent(Graphics g) 
    {
      super.paintComponent(g);

      for(int x = 0; x < width; ++x)
      {
        for(int y = 0; y < height; ++y)
        {
          int colorIntensity = imageData[x*y] & 0xff;
          //System.out.println("colorIntensity: " + Integer.toString(colorIntensity));
          g.setColor(new Color(colorIntensity, colorIntensity, colorIntensity));
          g.drawLine(x, y, x, y);
        }
      }

      g.setColor(randomColor);
      g.fillOval(70, 70, 100, 100);        
    }
}