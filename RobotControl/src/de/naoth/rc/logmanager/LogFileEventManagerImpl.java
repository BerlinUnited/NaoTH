/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

import de.naoth.rc.components.teamcomm.TeamCommManager;
import de.naoth.rc.components.teamcomm.TeamCommMessage;
import de.naoth.rc.dataformats.SPLMessage;
import de.naoth.rc.core.messages.TeamMessageOuterClass;
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
    
}
