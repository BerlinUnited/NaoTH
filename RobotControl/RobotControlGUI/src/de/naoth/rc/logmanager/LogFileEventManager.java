/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

import java.util.Collection;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface LogFileEventManager extends Plugin {

    void addListener(LogFrameListener l);

    void removeListener(LogFrameListener l);
    
    void fireLogFrameEvent(Collection<LogDataFrame> c);
}
