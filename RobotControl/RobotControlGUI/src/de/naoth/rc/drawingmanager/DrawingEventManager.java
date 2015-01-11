/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.drawingmanager;

import de.naoth.rc.drawings.Drawable;
import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author thomas
 */
public interface DrawingEventManager extends Plugin {

    void addListener(DrawingListener l);

    void removeListener(DrawingListener l);
    
    void fireDrawingEvent(Drawable drawing);
}
