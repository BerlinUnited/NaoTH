package de.naoth.rc.components.gamecontroller.event;

import de.naoth.rc.components.gamecontroller.GameControllerData;
import java.util.EventObject;

/**
 * Event object holding the GameController data.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class GameControllerDataEvent extends EventObject
{
    public final GameControllerData data;
    
    public GameControllerDataEvent(Object source, GameControllerData data) {
        super(source);
        this.data = data;
    }
}
