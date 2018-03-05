/*
 */
package de.naoth.rc.dataformats;

import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.messages.TeamMessageOuterClass;
import java.awt.Color;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;

/**
 *
 * @author Heinrich Mellmann
 */
public abstract class SPLMessage 
{
    public static final int SPL_MAXIMAL_MESSAGE_SIZE = 1024;
    
    public static SPLMessage parseFromBuffer(ByteBuffer buffer) throws Exception {
        if (   buffer.get() != 'S'
            || buffer.get() != 'P'
            || buffer.get() != 'L'
            || buffer.get() != ' ') {
            throw new Exception("Not an SPL Message.");
        }
        
        byte version = buffer.get();
        switch (version) {
            case SPLMessage2017.SPL_STANDARD_MESSAGE_STRUCT_VERSION:
                return new SPLMessage2017(buffer);
            case SPLMessage2018.SPL_STANDARD_MESSAGE_STRUCT_VERSION:
                return new SPLMessage2018(buffer);
            default:
                throw new Exception("Unknown message version: received " + version);
        }
    }
    
    public static SPLMessage parseFromProtobuf(TeamMessageOuterClass.TeamMessage.Data msg) {
        // HACK: determine which version it is
        if(msg.hasBallVelocity()) {
            return new SPLMessage2017(msg);
        } else {
            return new SPLMessage2018(msg);
        }
    }
    
    public <T> T getValue(String name, T default_value) {
        try {
            Field field = this.getClass().getField(name);
            return (T) field.get(this);
        } catch (NoSuchFieldException | SecurityException | IllegalAccessException ex) {
            return default_value;
        }
    }
    
    public boolean hasValue(String name) {
        try {
            Field field = this.getClass().getField(name);
            return field != null;
        } catch (NoSuchFieldException | SecurityException ex) {
            return false;
        }
    }
    
    public abstract int getPlayerNumber();
    public abstract int getTeamNumber();
    public abstract TeamMessageOuterClass.BUUserTeamMessage getUser();
    public abstract void draw(DrawingCollection drawings, Color robotColor, boolean mirror);
    
    /*
    public boolean getFallen() {
        return getValue("fallen", false);
    }
    */
}
