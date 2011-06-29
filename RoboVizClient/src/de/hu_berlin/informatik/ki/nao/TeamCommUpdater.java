/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.messages.Messages.TeamCommMessage;
import java.awt.Color;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Arrays;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;

public class TeamCommUpdater extends TimerTask{

  public final int NUM_OF_PLAYERS = 3;
  public final int TEAMCOMM_PORT = 10700;
  public final int MAX_PACKAGE_SIZE = 256;
  private DatagramSocket udpSocket;
  private RoboVizClient roboVizClient;

  public TeamCommUpdater(RoboVizClient rvc) throws SocketException
  {
    roboVizClient = rvc;
    udpSocket = new DatagramSocket(TEAMCOMM_PORT);
  }

  @Override
  public void run()
  {
    if (udpSocket != null && udpSocket.isBound())
    {
      try
      {
        byte[] buffer = new byte[MAX_PACKAGE_SIZE];
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
        // only check if something is there
//        udpSocket.setSoTimeout(10);
        udpSocket.receive(packet);
        byte[] truncatedBuffer = Arrays.copyOf(buffer, packet.getLength());
        TeamCommMessage msg = TeamCommMessage.parseFrom(truncatedBuffer);

        draw(msg);
      } catch (IOException ex)
      {
        Logger.getLogger(TeamCommUpdater.class.getName()).log(Level.SEVERE, null, ex);
      }
    }
  }

  public void draw(TeamCommMessage msg)
  {
    String group = "team" + msg.getTeamNumber() + ".player"+msg.getPlayerNumber();
//    System.out.println(group);

    if (msg.hasPositionOnField())
    {
      float[] p = new float[4];
      p[0] = (float) (msg.getPositionOnField().getTranslation().getX() * 1e-3);
      p[1] = (float) (msg.getPositionOnField().getTranslation().getY() * 1e-3);
      p[2] = 0;
      p[3] = (float) (msg.getPositionOnField().getRotation());
      drawPositionOnField(p, msg, group+".pose");
      
      p[2] = 0.2f;
      drawText(p, String.format("%.0f", msg.getTimeToBall()), group+".time2Ball");
    }
    
    try
    {
      roboVizClient.swapBuffers(group);
    } catch (IOException ex)
    {
      Logger.getLogger(TeamCommUpdater.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

  public void drawPositionOnField(float [] p, TeamCommMessage msg, String group)
  {
    try
    {
      Color c1 = Color.BLUE;
      Color c2 = Color.BLUE;

      if ( msg.hasWasStriker() && msg.getWasStriker() )
      {
        c1 = Color.RED;
      }
      else if ( msg.hasIsFallenDown() && msg.getIsFallenDown() )
      {
        c1 = Color.BLACK;
      }

      if ( msg.getTimeSinceBallWasSeen() > 1000 )
      {
        c2 = Color.BLACK;
      }

      final float radius = 0.2f;
      final float thickness = 3;
      
      roboVizClient.drawCircle(p, radius, thickness, c1, group);

      float[] d = new float[3];
      d[0] = (float) (p[0] + 2 * radius * Math.cos(p[3]));
      d[1] = (float) (p[1] + 2 * radius * Math.sin(p[3]));
      d[2] = 0;

      roboVizClient.drawLine(p, d, thickness, c2, group);
    } catch (IOException ex)
    {
      Logger.getLogger(TeamCommUpdater.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

  public void drawText(float [] p, String text, String group)
  {
    try
    {
      roboVizClient.drawAnnotation(text, p, Color.white, group);
    } catch (IOException ex)
    {
      Logger.getLogger(TeamCommUpdater.class.getName()).log(Level.SEVERE, null, ex);
    }
  }
}
