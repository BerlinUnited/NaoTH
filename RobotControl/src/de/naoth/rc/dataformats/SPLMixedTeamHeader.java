package de.naoth.rc.dataformats;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SPLMixedTeamHeader
{
    private int size = 0;
    public int getSize() { return size; }
    
    public long timestamp; // different for 32/64Bit => 4Byte vs 8Byte
    public byte teamID;
    public byte isPenalized;
    public byte whistleDetected;
    public byte dummy;

    public static SPLMixedTeamHeader parseDoBerManData(byte[] data) {
        // WARNING! HACK!
        // the struct size is different in 32/64 bit (long 32 to 64bit)
        // but it looks like, java can parse both, not matter which size ...
        SPLMixedTeamHeader mixed = null;
        if(data.length >= 16) {
            ByteBuffer doberHeader = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
            mixed = new SPLMixedTeamHeader();
            mixed.timestamp = doberHeader.getLong();
            mixed.teamID = doberHeader.get();
            mixed.isPenalized = doberHeader.get();
            mixed.whistleDetected = doberHeader.get();
            mixed.dummy = doberHeader.get();
            
            // NOTE: on x64 this is 16!
            mixed.size = 12;
        }

        return mixed;
    }
    
    public boolean isStriker;
    public byte role;
    
    public static SPLMixedTeamHeader parseBnBData(byte[] data)
    {
        SPLMixedTeamHeader mixed = null;
        ByteBuffer header = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        
        if(data.length >= 2) {
            mixed = new SPLMixedTeamHeader();
            
            byte rawData = header.get();
            mixed.isPenalized = (byte) ((rawData & 1) != 0 ? 1 : 0); // first bit set
            mixed.isStriker = (rawData & 2) != 0; // second bit set
            
            // NOTE: Bhuman has only 3 groups (defender, midfielder, forward)
            mixed.role = (byte) (header.get() * 3);
            
            mixed.size = 2;
        }
        
        return mixed;
    }
}
