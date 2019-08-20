package de.naoth.rc.dataformats;

import java.nio.ByteBuffer;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage2017 extends SPLMessage
{
    public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 6;
    public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 780;
    public static final int SPL_STANDARD_MESSAGE_SIZE = 70 + SPL_STANDARD_MESSAGE_DATA_SIZE;
    
    public SPLMessage2017(ByteBuffer buffer) throws Exception
    {
        this.playerNum = buffer.get();
        this.teamNum = buffer.get();
        this.fallen = buffer.get();
        this.pose_x = buffer.getFloat();
        this.pose_y = buffer.getFloat();
        this.pose_a = buffer.getFloat();
        this.walkingTo_x = buffer.getFloat();
        this.walkingTo_y = buffer.getFloat();
        this.shootingTo_x = buffer.getFloat();
        this.shootingTo_y = buffer.getFloat();
        this.ballAge = buffer.getFloat();
        this.ball_x = buffer.getFloat();
        this.ball_y = buffer.getFloat();
        this.ballVel_x = buffer.getFloat();
        this.ballVel_y = buffer.getFloat();

        // skip the suggestion array
        for (int i = 0; i < SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS; i++) {
            this.suggestion[i] = buffer.get();
        }

        this.intention = buffer.get();

        this.averageWalkSpeed = buffer.getShort();
        this.maxKickDistance = buffer.getShort();

        this.currentPositionConfidence = buffer.get();
        this.currentSideConfidence = buffer.get();

        this.numOfDataBytes = buffer.getShort();
        this.data = new byte[this.numOfDataBytes];
        buffer.get(this.data, 0, this.data.length);
        
        // parse mixed team header
        this.mixedHeader = SPLMixedTeamHeader.parseDoBerManData(this.data);
        
        // if we could parse the mixed team header ...
        if(this.mixedHeader != null) {
            // parse custom data
            if(!this.extractCustomData(mixedHeader.getSize())) {
                // custom data couldn't be parsed or key is wrong
                System.err.println("Something is wrong?! Invalid size?:" + mixedHeader.getSize());
            }
        }
    }
}
