/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class LogFileEventManagerImpl implements LogFileEventManager {
    private final List<LogFrameListener> listeners = new ArrayList<>();
    private final BlackBoardImpl blackBoard = new BlackBoardImpl();
    
    @Override
    public void addListener(LogFrameListener l) {
        listeners.add(l);
    }
    @Override
    public void removeListener(LogFrameListener l) {
        listeners.remove(l);
    }
    
    @Override
    public void fireLogFrameEvent(Collection<LogDataFrame> c) {
        
        if(c != null) {
            for(LogDataFrame f: c) {
                blackBoard.add(f);
            }
        }
        
        for(LogFrameListener l: listeners) {
            l.newFrame(blackBoard);
        }
    }
}
