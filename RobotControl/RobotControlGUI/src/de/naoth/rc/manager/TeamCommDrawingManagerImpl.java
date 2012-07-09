/*
 * 
 */

package de.naoth.rc.manager;

import com.google.protobuf.CodedOutputStream;
import com.google.protobuf.InvalidProtocolBufferException;
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
import de.naoth.rc.messages.Messages.TeamCommMessage;
import de.naoth.rc.server.Command;
import de.naoth.rc.server.CommandSender;
import de.naoth.rc.server.MessageServer;
import java.awt.Color;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
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
    public void addCommandSender(CommandSender commandSender) {
      // do nothing
    }
  }

  DummyServer dummyServer = new DummyServer();

  public TeamCommDrawingManagerImpl()
  {
    
  }

  private String currenId = null;
  private HashMap<String, ArrayList<Drawable>> drawingMap = new HashMap<String, ArrayList<Drawable>>();

  public void setCurrenId(String currenId) {
    this.currenId = currenId;
  }

  @Override
  public DrawingsContainer convertByteArrayToType(byte[] result) throws IllegalArgumentException
  {
    if(this.currenId == null) return null;

    DrawingsContainer drawingList = new DrawingsContainer();
    
    try{
      TeamCommMessage msg = TeamCommMessage.parseFrom(result);
      
      drawingMap.put(currenId, parseContainer(msg));

      for(ArrayList<Drawable> drawables: drawingMap.values())
      {
        for(Drawable drawing: drawables)
        {
          drawingList.add(drawing);
        }
      }

    }catch(InvalidProtocolBufferException ex)
    {
      throw new IllegalArgumentException(ex.toString());
    }
    catch(IOException iox)
    {
        iox.printStackTrace(System.err);
    }

    return drawingList;
  }//end convertByteArrayToType


  private ArrayList<Drawable> parseContainer(TeamCommMessage msg)
  {
    ArrayList<Drawable> drawingList = new ArrayList<Drawable>();
    drawingList.add(new DrawingOnField(null));

    Pose2D robotPose = new Pose2D();

    if(msg.hasPositionOnField())
    {
      Pen pen = new Pen(30, Color.gray);
      drawingList.add(pen);

      robotPose = new Pose2D(
              msg.getPositionOnField().getTranslation().getX(),
              msg.getPositionOnField().getTranslation().getY(),
              msg.getPositionOnField().getRotation());

      Robot robot = new Robot(
              robotPose.translation.x,
              robotPose.translation.y,
              robotPose.rotation);

      drawingList.add(robot);
    }

    if(msg.hasWasStriker() && msg.getWasStriker())
    {
      Pen pen = new Pen(30, Color.red);
      drawingList.add(pen);

      Circle marker = new Circle(
              (int)robotPose.translation.x,
              (int)robotPose.translation.y,
              150);

      drawingList.add(marker);
    }

    {
      Pen pen = new Pen(1, Color.black);
      drawingList.add(pen);

      // get the number
      int ix = this.currenId.lastIndexOf('.');
      String number = this.currenId.substring(ix);

      Text text = new Text(
              (int)robotPose.translation.x,
              (int)robotPose.translation.y+150,
              number+"(" + msg.getPlayerNumber() + ")");
      drawingList.add(text);
    }

    if(msg.hasBallPosition())
    {
      Pen pen = new Pen(1, Color.orange);
      drawingList.add(pen);


      Vector2D ballPos = new Vector2D(
              msg.getBallPosition().getX(),
              msg.getBallPosition().getY());

      Vector2D globalBall = robotPose.multiply(ballPos);

      FillOval ball = new FillOval(
              (int)globalBall.x,
              (int)globalBall.y,
              65,
              65);
      drawingList.add(ball);

      if(msg.hasTimeSinceBallWasSeen())
      {
        Pen pen2 = new Pen(1, Color.black);
        drawingList.add(pen2);
      
        double t = msg.getTimeSinceBallWasSeen() / 1000.0;

        Text text = new Text(
                (int)globalBall.x+50,
                (int)globalBall.y+50,
                Math.round(t)+"s");
        drawingList.add(text);
      }
    }

    return drawingList;
  }//end parseNewContainer

  @Override
  public Command getCurrentCommand()
  {
    return null;
  }

  @Override
  public MessageServer getServer()
  {
    return dummyServer;
  }
}//end TeamCommDrawingManager
