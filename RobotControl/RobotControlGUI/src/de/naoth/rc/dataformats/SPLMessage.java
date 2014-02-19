/*
 */

package de.naoth.rc.dataformats;

import java.nio.ByteBuffer;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage
{
  public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 1;
  public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 800;
  public static final int SPL_STANDARD_MESSAGE_SIZE = 46 + SPL_STANDARD_MESSAGE_DATA_SIZE;

  //public byte header[4]; // 4
  //public byte version; // 1
  public byte playerNum; // 1
  public short team; // 2  // 0 is red 1 is blue
  public float pose_x; // 4
  public float pose_y; // 4
  public float pose_a; // 4
  public int ballAge; // 4
  public float ball_x; // 4
  public float ball_y; // 4
  public float ballVel_x; // 4
  public float ballVel_y; // 4
  public int fallen; // 4
  public short numOfDataBytes; // 2
  public byte[] data;

  public SPLMessage(ByteBuffer buffer)
  {
      if( buffer.get() != 'S' ||
          buffer.get() != 'P' ||
          buffer.get() != 'L' ||
          buffer.get() != ' ')
      {
          System.out.println("Not an SPL Message.");
          return;
      }

      byte version = buffer.get();
      if(version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
      {
          System.out.println("Wrong version " + version + ", expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION);
          return;
      }

      this.playerNum = buffer.get();
      this.team = buffer.getShort();
      this.pose_x = buffer.getFloat();
      this.pose_y = buffer.getFloat();
      this.pose_a = buffer.getFloat();
      this.ballAge = buffer.getInt();
      this.ball_x = buffer.getFloat();
      this.ball_y = buffer.getFloat();
      this.ballVel_x = buffer.getFloat();
      this.ballVel_y = buffer.getFloat();
      this.fallen = buffer.getInt();
      this.numOfDataBytes = buffer.getShort();
      this.data = new byte[this.numOfDataBytes];
      buffer.get(this.data, 0, this.data.length);
  }
}
