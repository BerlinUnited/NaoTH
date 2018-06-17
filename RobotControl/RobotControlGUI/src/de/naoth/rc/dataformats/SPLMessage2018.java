package de.naoth.rc.dataformats;

import java.nio.ByteBuffer;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage2018 extends SPLMessage
{
    public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 7;
    public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 474;
    public static final int SPL_STANDARD_MESSAGE_SIZE = 70 + SPL_STANDARD_MESSAGE_DATA_SIZE;

    public SPLMessage2018(ByteBuffer buffer) throws Exception
    {
        this.playerNum = buffer.get();
        this.teamNum = buffer.get();
        this.fallen = buffer.get();
        this.pose_x = buffer.getFloat();
        this.pose_y = buffer.getFloat();
        this.pose_a = buffer.getFloat();
        this.ballAge = buffer.getFloat();
        this.ball_x = buffer.getFloat();
        this.ball_y = buffer.getFloat();

        this.numOfDataBytes = buffer.getShort();
        this.data = new byte[this.numOfDataBytes];
        buffer.get(this.data, 0, this.data.length);
        
        // parse mixed team header
        this.doberHeader = DoBerManCustomHeader.parseData(this.data);
        
        // if we could parse the mixed team header ...
        if(this.doberHeader != null) {
            // parse our own part ...
            // WARNING! HACK!
            // the struct size is different in 32/64 bit
            if(!this.extractCustomData(BU_CUSTOM_DATA_OFFSET_X32)) {
                this.extractCustomData(BU_CUSTOM_DATA_OFFSET_X64);
            }
        }
        
        if(this.user != null) {
            // since it is not contained in the std. message any more, we need to get it from the custom part
            this.intention = (byte) (this.user.getWasStriker() ? 3 : 0);
            this.ballVel_x = (float) this.user.getBallVelocity().getX();
            this.ballVel_y = (float) this.user.getBallVelocity().getY();
        }
    }
}
