/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao;

import de.hu_berlin.informatik.ki.nao.rv.RVDraw;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.awt.Color;
import java.io.IOException;
import java.net.DatagramPacket;

public class RoboVizClient
{
  private static final int ROBOVIS_PORT = 32769;
  private DatagramSocket socket;
  private InetAddress      address;

  public RoboVizClient() throws SocketException, UnknownHostException
  {
    socket = new DatagramSocket();
    address = InetAddress.getLocalHost();
  }

  public void swapBuffers(String group) throws IOException
  {
    byte[] buf = RVDraw.newBufferSwap(group);
    socket.send(new DatagramPacket(buf, buf.length, address, ROBOVIS_PORT));
  }

  public void drawCircle(float[] center, float radius, float thickness,
          Color color, String group) throws IOException
  {
    byte[] buf = RVDraw.newCircle(center, radius, thickness, color, group);
    socket.send(new DatagramPacket(buf, buf.length, address, ROBOVIS_PORT));
  }

  public void drawLine(float[] a, float[] b, float thickness, Color color,
          String group) throws IOException
  {
    byte[] buf = RVDraw.newLine(a, b, thickness, color, group);
    socket.send(new DatagramPacket(buf, buf.length, address, ROBOVIS_PORT));
  }

  public void drawAnnotation(String text, float[] pos, Color color, String set)
          throws IOException
  {
    byte[] buf = RVDraw.newAnnotation(text, pos, color, set);
    socket.send(new DatagramPacket(buf, buf.length, address, ROBOVIS_PORT));
  }
}
