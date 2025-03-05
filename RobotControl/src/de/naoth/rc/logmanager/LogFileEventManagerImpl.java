/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import de.naoth.rc.dataformats.SPLMessage;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import javax.swing.SwingUtilities;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class LogFileEventManagerImpl implements LogFileEventManager
{
    private final List<LogFrameListener> listeners = new ArrayList<>();
    private final BlackBoardImpl blackBoard = new BlackBoardImpl();
    
    private final ReadWriteLock lock = new ReentrantReadWriteLock();
    
    @InjectPlugin
    public static TeamCommManager teamcommManager;
    
    @Override
    public void addListener(LogFrameListener l) {
      lock.writeLock().lock();
      try
      {
        listeners.add(l);
      }
      finally
      {
        lock.writeLock().unlock();
      }
    }
    @Override
    public void removeListener(LogFrameListener l) {
      lock.writeLock().lock();
      try
      {
        listeners.remove(l);
      }
      finally
      {
        lock.writeLock().unlock();
      }
    }
    
    @Override
    public void fireLogFrameEvent(final Collection<LogDataFrame> c) {
      
      if(SwingUtilities.isEventDispatchThread()) {
        internalFire(c);
      }
      else {
        SwingUtilities.invokeLater(new Runnable()
        {

          @Override
          public void run()
          {
            internalFire(c);
          }
        });
      }
       
    }
    
    private void internalFire(Collection<LogDataFrame> c) {
       if(c != null) {
            List<TeamCommMessage> messages = new java.util.ArrayList<>();
            for(LogDataFrame f: c) {
                blackBoard.add(f);
                if(f.getName().equals("TeamMessage")) {
                    try {
                        TeamMessageOuterClass.TeamMessage.parseFrom(f.getData()).getDataList().stream().forEach(msg->{
                            SPLMessage spl = SPLMessage.parseFrom(msg);
                            messages.add(new TeamCommMessage(
                                System.currentTimeMillis(),
                                "10.0."+spl.teamNum+"."+spl.playerNum, // artificially set an ip
                                spl,
                                spl.teamNum != 4) // TOOD: can we set anywhere our team number?!?
                            );
                        });
                    } catch (Exception e) {}
                }
                else if (f.getName().equals("TeamState"))
                {
                    try
                    {
                        TeamMessageOuterClass.TeamState d = TeamMessageOuterClass.TeamState.parseFrom(f.getData());
                        d.getPlayersList().forEach(p ->
                        {
                            SPLMessage spl = createSplMessageFromTeamState(p);
                            messages.add(new TeamCommMessage(
                                    System.currentTimeMillis(),
                                    "10.0." + spl.teamNum + "." + spl.playerNum, // artificially set an ip
                                    spl,
                                    spl.teamNum != 4) // TOOD: can we set anywhere our team number?!?
                            );
                        });
                    }
                    catch (Exception e)
                    {
                    }
                }
            }
            if(!messages.isEmpty()) {
                teamcommManager.receivedMessages(messages);
            }
        }
        
        lock.readLock().lock();
        try
        {
          for(LogFrameListener l: listeners) {
              l.newFrame(blackBoard);
          }
        }
        finally
        {
          lock.readLock().unlock();
        }
    }

    private SPLMessage createSplMessageFromTeamState(TeamMessageOuterClass.TeamState.Player user)
    {
        SPLMessage spl_msg = new SPLMessage();
        spl_msg.playerNum = (byte) user.getNumber();
        spl_msg.teamNum = (byte) 4; // we've got no team number in the team state representation ...
        spl_msg.fallen = (byte) (user.getFallen() ? 1 : 0);
        spl_msg.pose_x = (float) user.getPose().getTranslation().getX();
        spl_msg.pose_y = (float) user.getPose().getTranslation().getY();
        spl_msg.pose_a = (float) user.getPose().getRotation();
        spl_msg.ballAge = user.getBallAge();
        spl_msg.ball_x = (float) user.getBallPosition().getX();
        spl_msg.ball_y = (float) user.getBallPosition().getY();

        TeamMessageOuterClass.BUUserTeamMessage.Builder b = TeamMessageOuterClass.BUUserTeamMessage.newBuilder()
                //.setBodyID(user.getBodyID())
                .setTimeToBall(user.getTimeToBall())
                .setWasStriker(user.getWasStriker())
                .setWantsToBeStriker(user.getWantsToBeStriker())
                //.setIsPenalized()
                //.addOpponents()
                //.setTeamNumber(user.getTeamNumber())
                //.setBatteryCharge(user.getBatteryCharge())
                //.setTemperature(user.getTemperature())
                .setTimestamp(user.getMessageTimestamp())
                //.setWantsToBeStriker()
                //.setCpuTemperature(user.getCpuTemperature())
                //.setWhistleDetected(user.getWhistleDetected())
                //.setWhistleCount(user.getWhistleCount())
                //.setTeamBall(user.getTeamBall())
                //.addNtpRequest(builderForValue)
                //.setBallVelocity(user.getBallVelocity())
                .setRobotState(user.getRobotState())
                .setRobotRole(user.getRobotRole())
                .setReadyToWalk(user.getReadyToWalk())
                .setKey("naoth");
        user.getNtpRequestList().forEach((n -> b.addNtpRequest(n)));
        spl_msg.user = b.build();

        return spl_msg;
    }
}
