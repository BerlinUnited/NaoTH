/*
 */

package de.naoth.rc.drawingmanager;

import de.naoth.rc.dialogs.drawings.Drawable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import net.xeoh.plugins.base.annotations.PluginImplementation;

/**
 *
 * @author thomas
 */
@PluginImplementation
public class DrawingEventManagerImpl implements DrawingEventManager {
    private final List<DrawingListener> listeners = Collections.synchronizedList(new ArrayList<DrawingListener>());
    
    @Override
    public void addListener(DrawingListener l) {
        listeners.add(l);
    }
    @Override
    public void removeListener(DrawingListener l) {
        listeners.remove(l);
    }
    
    @Override
    public synchronized void fireDrawingEvent(Drawable drawing) {
        for(DrawingListener l: listeners) {
            l.newDrawing(drawing);
        }
    }
}
