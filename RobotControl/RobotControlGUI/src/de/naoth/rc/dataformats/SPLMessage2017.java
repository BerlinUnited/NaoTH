/*
 */
package de.naoth.rc.dataformats;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.drawings.Arrow;
import de.naoth.rc.drawings.Circle;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.FillOval;
import de.naoth.rc.drawings.Line;
import de.naoth.rc.drawings.Pen;
import de.naoth.rc.drawings.Robot;
import de.naoth.rc.drawings.Text;
import de.naoth.rc.math.Pose2D;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.messages.TeamMessageOuterClass;
import java.awt.BasicStroke;
import java.awt.Color;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage2017 extends SPLMessage {

    public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 6;
    public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 780;
    public static final int SPL_STANDARD_MESSAGE_SIZE = 70 + SPL_STANDARD_MESSAGE_DATA_SIZE;
    public static final int SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 5;
    
    public static final int BU_CUSTOM_DATA_OFFSET_X32 = 12;
    public static final int BU_CUSTOM_DATA_OFFSET_X64 = 16;

    public static class DoBerManCustomHeader
    {
        public long timestamp;
        public byte teamID;
        public byte isPenalized;
        public byte whistleDetected;
        public byte dummy;
        
        public static DoBerManCustomHeader parseData(byte[] data) {
            // WARNING! HACK!
            // the struct size is different in 32/64 bit
            // but it looks like, java can parse both, not matter which size ...
            DoBerManCustomHeader mixed = null;
            if(data.length >= BU_CUSTOM_DATA_OFFSET_X64) {
                ByteBuffer doberHeader = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
                mixed = new DoBerManCustomHeader();
                mixed.timestamp = doberHeader.getLong();
                mixed.teamID = doberHeader.get();
                mixed.isPenalized = doberHeader.get();
                mixed.whistleDetected = doberHeader.get();
                mixed.dummy = doberHeader.get();
            }
            
            return mixed;
        }
    }
    
    //public byte header[4]; // 4
    //public byte version; // 1
    public byte playerNum; // 1
    public byte teamNum; // 1 
    public byte fallen; // 1
    public float pose_x; // 4
    public float pose_y; // 4
    public float pose_a; // 4

    public float walkingTo_x; // 4
    public float walkingTo_y; // 4

    public float shootingTo_x; // 4
    public float shootingTo_y; // 4

    public float ballAge; // 4
    public float ball_x; // 4
    public float ball_y; // 4
    public float ballVel_x; // 4
    public float ballVel_y; // 4

    public byte[] suggestion = new byte[SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS]; // 5

    // describes what the robot intends to do:
    // 0 - nothing particular (default)
    // 1 - wants to be keeper
    // 2 - wants to play defense
    // 3 - wants to play the ball
    // 4 - robot is lost
    // (the second byte is a padding byte)
    public byte intention; // 1

    public short averageWalkSpeed; // 2
    public short maxKickDistance; // 2

    // [MANDATORY]
    // describes the current confidence of a robot about its self-location,
    // the unit is percent [0,..100]
    // the value should be updated in the course of the game
    public byte currentPositionConfidence; // 1

    // [MANDATORY]
    // describes the current confidence of a robot about playing in the right direction,
    // the unit is percent [0,..100]
    // the value should be updated in the course of the game
    public byte currentSideConfidence; // 1

    public short numOfDataBytes; // 2

    public byte[] data;

    public transient TeamMessageOuterClass.BUUserTeamMessage user = null;
    
    public transient DoBerManCustomHeader doberHeader = null;

    public SPLMessage2017()
    {
        user = TeamMessageOuterClass.BUUserTeamMessage.getDefaultInstance();
    }
    
    public SPLMessage2017(TeamMessageOuterClass.TeamMessage.Data msg) {

        this.averageWalkSpeed = -1;

        this.ballAge = msg.getBallAge();

        this.ballVel_x = (float) msg.getBallVelocity().getX();
        this.ballVel_y = (float) msg.getBallVelocity().getY();

        this.ball_x = (float) msg.getBallPosition().getX();
        this.ball_y = (float) msg.getBallPosition().getY();

        this.currentPositionConfidence = -1;
        this.currentSideConfidence = -1;

        this.fallen = msg.getFallen() ? (byte) 1 : (byte) 0;

        this.intention = (byte) (msg.getUser().getWasStriker() ? 3 : 0);
        this.maxKickDistance = -1;

        this.playerNum = (byte) msg.getPlayerNum();

        this.pose_x = (float) msg.getPose().getTranslation().getX();
        this.pose_y = (float) msg.getPose().getTranslation().getY();
        this.pose_a = (float) msg.getPose().getRotation();

        // TODO: don't use default value
        this.shootingTo_x = (float) msg.getPose().getTranslation().getX();
        this.shootingTo_y = (float) msg.getPose().getTranslation().getY();

        if (msg.hasTeamNumber()) {
            this.teamNum = (byte) msg.getTeamNumber();
        } else if (msg.getUser().hasTeamNumber()) {
            this.teamNum = (byte) msg.getTeamNumber();
        }

        this.user = msg.getUser();

        this.walkingTo_x = -1;
        this.walkingTo_y = -1;
    }

    public SPLMessage2017(ByteBuffer buffer) throws Exception {
        /*
        if (buffer.get() != 'S'
            || buffer.get() != 'P'
            || buffer.get() != 'L'
            || buffer.get() != ' ') {
            throw new Exception("Not an SPL Message.");
            //System.out.println("Not an SPL Message.");
        }

        byte version = buffer.get();
        if (version != SPL_STANDARD_MESSAGE_STRUCT_VERSION) {
            throw new Exception("Wrong version: reveived " + version + ", but expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION);
            //System.out.println("Wrong version " + version + ", expected " + SPL_STANDARD_MESSAGE_STRUCT_VERSION);
        }
        */

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
    }

    public void draw(DrawingCollection drawings, Color robotColor, boolean mirror) {
        
        
        Vector2D ballPos = new Vector2D(ball_x, ball_y);
        Pose2D robotPose = mirror ? new Pose2D(-pose_x, -pose_y, pose_a + Math.PI)
            : new Pose2D(pose_x, pose_y, pose_a);
        
        Vector2D globalBall = robotPose.multiply(ballPos);
        
        // robot
        drawings.add(new Pen(1.0f, robotColor));
        drawings.add(new Robot(robotPose.translation.x, robotPose.translation.y, robotPose.rotation));
        
        // number
        drawings.add(new Pen(1, Color.BLACK));
        drawings.add(new Text((int) robotPose.translation.x, (int) robotPose.translation.y + 150, "" + playerNum));


        // striker
        if (intention == 3) {
            drawings.add(new Pen(30, Color.red));
            drawings.add(new Circle((int) robotPose.translation.x, (int) robotPose.translation.y, 150));
        }

        
        // ball
        if (ballAge >= 0 && ballAge < 3) {
            drawings.add(new Pen(1, Color.orange));

            drawings.add(new FillOval((int) globalBall.x, (int) globalBall.y, 65, 65));

            // add a surrounding black circle so the ball is easier to see
            drawings.add(new Pen(1, Color.black));
            drawings.add(new Circle((int) globalBall.x, (int) globalBall.y, 65));
            
            {
                // show the time since the ball was last seen
                drawings.add(new Pen(1, Color.black));
                double t = ballAge;

                Text text = new Text(
                    (int) globalBall.x + 50,
                    (int) globalBall.y + 50,
                    Math.round(t) + "s");
                drawings.add(text);
            }

            // draw a line between robot and ball
            {
                drawings.add(new Pen(5, Color.darkGray));
                Line ballLine = new Line(
                    (int) robotPose.translation.x, (int) robotPose.translation.y,
                    (int) globalBall.x, (int) globalBall.y);
                drawings.add(ballLine);
            }
            
            // if it is our striker ...
            if(user != null && intention == 3 && shootingTo_x != 0 && shootingTo_y != 0)
            {
                // ... draw the excpected ball position
                drawings.add(new Pen(5.0f, Color.gray));
                drawings.add(new Circle((int) shootingTo_x, (int) shootingTo_y, 65));
                drawings.add(new Pen(Color.gray, new BasicStroke(10, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 0, new float[]{100, 50}, 0)));
                drawings.add(new Arrow((int) globalBall.x, (int) globalBall.y, (int) shootingTo_x, (int) shootingTo_y));
            }
        }
        
        // if it is our player ...
        if(user != null)
        {
            double[] tb = {user.getTeamBall().getX(), user.getTeamBall().getY()};
            if(!Double.isInfinite(tb[0]) && !Double.isInfinite(tb[1])) {
                // ... draw the teamball position
                drawings.add(new Pen(5.0f, robotColor));
                drawings.add(new Circle((int) tb[0], (int) tb[1], 65));
                drawings.add(new Pen(robotColor, new BasicStroke(10, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 0, new float[]{25, 50, 75, 100}, 0)));
                drawings.add(new Arrow((int) robotPose.translation.x, (int) robotPose.translation.y, (int) tb[0], (int) tb[1]));
            }
        }
    }
    
    public boolean parseCustomFromData() {
        // TODO: check if this needs to be changed - 'cause of dobermanheader!?!
        if(data != null) {
            try {
                user = TeamMessageOuterClass.BUUserTeamMessage.parseFrom(data);
                return true;
            } catch (InvalidProtocolBufferException ex) { /* ignore! instead "false" is returned. */ }
        }
        return false;
    }

    public final boolean extractCustomData(int size) {
        if(this.data.length > size) {
            byte[] dataWithOffset = Arrays.copyOfRange(this.data, size, this.data.length);
            try {
                this.user = TeamMessageOuterClass.BUUserTeamMessage.parseFrom(dataWithOffset);
                // additionally check if the magic string matches
                if(!"naoth".equals(this.user.getKey())) {
                    this.user = null;
                }
                return this.user != null;
            } catch (InvalidProtocolBufferException ex) {
                // it's not our message
            }
        }
        return false;
    }
    
    @Override
    public int getPlayerNumber() {
        return playerNum;
    }
    
    @Override
    public int getTeamNumber() {
        return teamNum;
    }
    
    @Override
    public TeamMessageOuterClass.BUUserTeamMessage getUser() {
        return this.user;
    }
}
