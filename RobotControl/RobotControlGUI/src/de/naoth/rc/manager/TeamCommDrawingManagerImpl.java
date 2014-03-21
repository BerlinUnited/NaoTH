/*
 * 
 */

package de.naoth.rc.manager;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.dialogs.TeamCommViewer;
import de.naoth.rc.dialogs.drawings.Circle;
import de.naoth.rc.dialogs.drawings.Drawable;
import de.naoth.rc.dialogs.drawings.DrawingOnField;
import de.naoth.rc.dialogs.drawings.DrawingsContainer;
import de.naoth.rc.dialogs.drawings.FillOval;
import de.naoth.rc.dialogs.drawings.Pen;
import de.naoth.rc.dialogs.drawings.Robot;
import de.naoth.rc.dialogs.drawings.Text;
import de.naoth.rc.math.Pose2D;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.messages.Representations;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.MessageServer;
import java.awt.Color;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author Heinrich Mellmann
 */
@PluginImplementation
public class TeamCommDrawingManagerImpl extends AbstractManager<DrawingsContainer>
        implements TeamCommDrawingManager
{

  private static class DummyServer extends MessageServer
  {
    @Override
    public boolean isConnected()
    {
       return true;
    }

    @Override
    public void connect(String host, int port) throws IOException
    {
      // do nothing
    }

    @Override
    public void disconnect()
    {
      // do nothing
    }
  }
 
  
  private final DummyServer dummyServer = new DummyServer();
  private DrawingsContainer drawings;
  
  public TeamCommDrawingManagerImpl()
  {
  }

  @Override
  public void handleSPLMessages(Map<String, SPLMessage> messages)
  {
    drawings = new DrawingsContainer();
    
    for(SPLMessage msg : messages.values())
  {
      // add the drawings for this message/player
      for(Drawable drawing : parseContainer(msg))
      {
        drawings.add(drawing);
      }
    }
    
    // HACK: trigger distribution of the message
    handleResponse(null, null);
  }

  
  
  @Override
  public DrawingsContainer convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    return drawings;

  }//end convertByteArrayToType


  private ArrayList<Drawable> parseContainer(SPLMessage msg)
  {
    ArrayList<Drawable> drawingList = new ArrayList<Drawable>();
    drawingList.add(new DrawingOnField(null));

    Pose2D robotPose = new Pose2D();

    {
      Pen pen = new Pen(30, Color.gray);
      drawingList.add(pen);
    }
    robotPose = new Pose2D(msg.pose_x,
            msg.pose_y,
            msg.pose_a);

    Robot robot = new Robot(
            robotPose.translation.x,
            robotPose.translation.y,
            robotPose.rotation);

    drawingList.add(robot);

     try
    {
      Representations.BUUserTeamMessage bumsg = Representations.BUUserTeamMessage.parseFrom(msg.data);
      
      if(bumsg.hasWasStriker() && bumsg.getWasStriker())
      {
        Pen pen = new Pen(30, Color.red);
        drawingList.add(pen);

        Circle marker = new Circle(
                (int)robotPose.translation.x,
                (int)robotPose.translation.y,
                150);

        drawingList.add(marker);
      }
      
      Pen pen = new Pen(1, Color.black);
      drawingList.add(pen);
      
    }
    catch(InvalidProtocolBufferException ex)
    {
      Logger.getLogger(TeamCommViewer.class.getName()).log(Level.INFO, "Invalid user team-message", ex);
    }
   
    // get the number
    {

      Text text = new Text(
        (int) robotPose.translation.x,
        (int) robotPose.translation.y + 150,
        (msg.team == 0 ? "blue" : "red") + " " + msg.playerNum);
      drawingList.add(text);
    }

    // ball
    if(msg.ballAge >= 0)
    {
      Pen pen = new Pen(1, Color.orange);
      drawingList.add(pen);

      Vector2D ballPos = new Vector2D(msg.ball_x,
        msg.ball_y);

      Vector2D globalBall = robotPose.multiply(ballPos);

      FillOval ball = new FillOval(
        (int) globalBall.x,
        (int) globalBall.y,
        65,
        65);
      drawingList.add(ball);
      
      Pen pen2 = new Pen(1, Color.black);
      drawingList.add(pen2);

      {
        double t = msg.ballAge / 1000.0;

        Text text = new Text(
          (int) globalBall.x + 50,
          (int) globalBall.y + 50,
          Math.round(t) + "s");
        drawingList.add(text);
      }
    }

    return drawingList;
  }//end parseNewContainer

  @Override
  public Command getCurrentCommand()
  {
    return new Command("dummy_teamcomm_command");
  }

  @Override
  public MessageServer getServer()
  {
    return dummyServer;
  }
}//end TeamCommDrawingManager
