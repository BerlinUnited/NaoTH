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
  public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 4;
  public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 802;
  public static final int SPL_STANDARD_MESSAGE_SIZE = 58 + SPL_STANDARD_MESSAGE_DATA_SIZE;

  //public byte header[4]; // 4
  //public byte version; // 1
  public byte playerNum; // 1
  public byte team; // 1 // 0 is red 1 is blue
  public byte fallen; // 1
  public float pose_x; // 4
  public float pose_y; // 4
  
  public float walkingTo_x; // 4
  public float walkingTo_y; // 4
  
  public float shootingTo_x; // 4
  public float shootingTo_y; // 4
  
  public float pose_a; // 4
  public int ballAge; // 4
  public float ball_x; // 4
  public float ball_y; // 4
  public float ballVel_x; // 4
  public float ballVel_y; // 4
  public short numOfDataBytes; // 2
  public byte[] data;

  public SPLMessage(ByteBuffer buffer) throws Exception
  {
      if( buffer.get() != 'S' ||
          buffer.get() != 'P' ||
          buffer.get() != 'L' ||
          buffer.get() != ' ')
      {
          throw new Exception("Not an SPL Message.");
          //System.out.println("Not an SPL Message.");
      }

      byte version = buffer.get();
      if(version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
      {
          throw new Exception("Wrong version: reveived " + version + ", but expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION);
          //System.out.println("Wrong version " + version + ", expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION);
      }

      this.playerNum = buffer.get();
      this.team = buffer.get();
      this.fallen = buffer.get();
      this.pose_x = buffer.getFloat();
      this.pose_y = buffer.getFloat();
      this.pose_a = buffer.getFloat();
      this.walkingTo_x = buffer.getFloat();
      this.walkingTo_y = buffer.getFloat();
      this.shootingTo_x = buffer.getFloat();
      this.shootingTo_y = buffer.getFloat();
      this.ballAge = buffer.getInt();
      this.ball_x = buffer.getFloat();
      this.ball_y = buffer.getFloat();
      this.ballVel_x = buffer.getFloat();
      this.ballVel_y = buffer.getFloat();
      this.numOfDataBytes = buffer.getShort();
      this.data = new byte[this.numOfDataBytes];
      buffer.get(this.data, 0, this.data.length);
  }
}
