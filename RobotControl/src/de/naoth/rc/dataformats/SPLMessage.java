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
import de.naoth.rc.core.messages.TeamMessageOuterClass;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage
{
    public static final int SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 5;
    /** List of supported versions */
    public static final List<Integer> SPL_MESSAGE_VERSIONS = Arrays.asList(
        SPLMessage2017.SPL_STANDARD_MESSAGE_STRUCT_VERSION,
        SPLMessage2018.SPL_STANDARD_MESSAGE_STRUCT_VERSION
    );

    //public byte header[4]; // 4
    //public byte version; // 1
    public byte playerNum; // 1
    public byte teamNum; // 1 
    public byte fallen; // 1
    public float pose_x; // 4
    public float pose_y; // 4
    public float pose_a; // 4
    @Deprecated
    public float walkingTo_x; // 4
    @Deprecated
    public float walkingTo_y; // 4
    @Deprecated
    public float shootingTo_x; // 4
    @Deprecated
    public float shootingTo_y; // 4

    public float ballAge; // 4
    public float ball_x; // 4
    public float ball_y; // 4
    @Deprecated
    public float ballVel_x; // 4
    @Deprecated
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
    
    public transient SPLMixedTeamHeader mixedHeader = null;
    
    public SPLMessage()
    {
        user = TeamMessageOuterClass.BUUserTeamMessage.getDefaultInstance();
    }
    
    public static SPLMessage parseFrom(TeamMessageOuterClass.TeamMessage.Data msg)
    {
        SPLMessage spl = new SPLMessage();
        spl.averageWalkSpeed = -1;

        spl.ballAge = msg.hasBallAge() ? msg.getBallAge() : -1;

        
        spl.ballVel_x = msg.hasBallVelocity() ? (float) msg.getBallVelocity().getX() : 0.0f;
        spl.ballVel_y = msg.hasBallVelocity() ? (float) msg.getBallVelocity().getY() : 0.0f;

        spl.ball_x = (float) msg.getBallPosition().getX();
        spl.ball_y = (float) msg.getBallPosition().getY();

        spl.currentPositionConfidence = -1;
        spl.currentSideConfidence = -1;

        spl.fallen = msg.getFallen() ? (byte) 1 : (byte) 0;

        spl.intention = (byte) (msg.getUser().getWasStriker() ? 3 : 0);
        spl.maxKickDistance = -1;

        spl.playerNum = (byte) msg.getPlayerNum();

        spl.pose_x = (float) msg.getPose().getTranslation().getX();
        spl.pose_y = (float) msg.getPose().getTranslation().getY();
        spl.pose_a = (float) msg.getPose().getRotation();

        // TODO: don't use default value
        spl.shootingTo_x = (float) msg.getPose().getTranslation().getX();
        spl.shootingTo_y = (float) msg.getPose().getTranslation().getY();

        if (msg.hasTeamNumber()) {
            spl.teamNum = (byte) msg.getTeamNumber();
        } else if (msg.getUser().hasTeamNumber()) {
            spl.teamNum = (byte) msg.getTeamNumber();
        }

        spl.user = msg.getUser();

        spl.walkingTo_x = -1;
        spl.walkingTo_y = -1;
        
        return spl;
    }
    
    public static SPLMessage parseFrom(ByteBuffer buffer) throws Exception {
        // check message header
        if (buffer.get() != 'S'
            || buffer.get() != 'P'
            || buffer.get() != 'L'
            || buffer.get() != ' ') {
            throw new NotSplMessageException();
        }
        
        int version = buffer.get();
        if(version == SPL_MESSAGE_VERSIONS.get(0)) {
            // parse with v2017
            return new SPLMessage2017(buffer);
        } else if(version == SPL_MESSAGE_VERSIONS.get(1)) {
            // parse with v2018
            return new SPLMessage2018(buffer);
        } else {
            throw new WrongSplVersionException(version);
        }
    }

    public void draw(DrawingCollection drawings, Color robotColor, boolean mirror)
    {
        // put the penalized players on "the bench"
        if(user != null &&
            ((user.hasIsPenalized() && user.getIsPenalized()) || (user.hasRobotState() && user.getRobotState() == TeamMessageOuterClass.RobotState.penalized)))
        {
            Pose2D robotPose = mirror ? new Pose2D(4600 - (playerNum * 500), -3300, Math.PI/2) : new Pose2D(-4600 + (playerNum * 500), 3300, -Math.PI/2);
            // robot
            drawings.add(new Pen(1.0f, robotColor));
            drawings.add(new Robot(robotPose.translation.x, robotPose.translation.y, robotPose.rotation));

            // number
            drawings.add(new Pen(1, Color.RED));
            Font numberFont = new Font ("Courier New", Font.PLAIN | Font.CENTER_BASELINE, 250);
            drawings.add(new Text((int) robotPose.translation.x, (int) robotPose.translation.y + 250, 0, "" + playerNum, numberFont));
            
            // don't draw anything else
            return;
        }
        
        Vector2D ballPos = new Vector2D(ball_x, ball_y);
        Pose2D robotPose = mirror ? new Pose2D(-pose_x, -pose_y, pose_a + Math.PI)
            : new Pose2D(pose_x, pose_y, pose_a);
        
        Vector2D globalBall = robotPose.multiply(ballPos);
        
        // robot
        drawings.add(new Pen(1.0f, robotColor));
        drawings.add(new Robot(robotPose.translation.x, robotPose.translation.y, robotPose.rotation));
        
        // number, use different colors for the different states
        switch (user.getRobotState()) {
            case initial: drawings.add(new Pen(1, Color.WHITE)); break;
            case ready: drawings.add(new Pen(1, Color.BLUE)); break;
            case set: drawings.add(new Pen(1, Color.YELLOW)); break;
            case playing: drawings.add(new Pen(1, Color.BLACK)); break;
            case finished: drawings.add(new Pen(1, Color.LIGHT_GRAY)); break;
            case penalized: drawings.add(new Pen(1, Color.RED)); break;
            default: drawings.add(new Pen(1, Color.PINK)); break;
        }
        Font numberFont = new Font ("Courier New", Font.PLAIN | Font.CENTER_BASELINE, 250);
        double fontRotation = 0;//robotPose.rotation+Math.PI/2; // rotate in the direction of the robots
        drawings.add(new Text((int) robotPose.translation.x, (int) robotPose.translation.y + 250, fontRotation, "" + playerNum, numberFont));

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
            drawings.add(new Pen(1, robotColor));
            drawings.add(new Circle((int) globalBall.x, (int) globalBall.y, 65));
            
            {
                // show the time since the ball was last seen
                drawings.add(new Pen(1, robotColor));
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
            if(user.hasTeamBall() && !Double.isInfinite(tb[0]) && !Double.isInfinite(tb[1])) {
                // ... draw the teamball position
                drawings.add(new Pen(5.0f, robotColor));
                drawings.add(new Circle((int) tb[0], (int) tb[1], 65));
                drawings.add(new Pen(robotColor, new BasicStroke(10, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 0, new float[]{25, 50, 75, 100}, 0)));
                drawings.add(new Arrow((int) robotPose.translation.x, (int) robotPose.translation.y, (int) tb[0], (int) tb[1]));
            }
            
            if(user.getWantsToBeStriker() && !user.getWasStriker()) {
                drawings.add(new Pen(32, Color.YELLOW));
                drawings.add(new Circle((int) robotPose.translation.x, (int) robotPose.translation.y, 150));
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

    protected boolean extractCustomData(int size) {
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
    
    public static int size() {
        return Integer.max(SPLMessage2017.SPL_STANDARD_MESSAGE_SIZE, SPLMessage2018.SPL_STANDARD_MESSAGE_SIZE);
    }
    
    /**
     * Exception for invalid spl message header.
     */
    public static class NotSplMessageException extends Exception {
        public NotSplMessageException() {
            super("Not an SPL Message.");
        }
    }
    
    /**
     * Exception for invalid/unknown spl message version.
     */
    public static class WrongSplVersionException extends Exception {
        public WrongSplVersionException(int version) {
            super("Wrong version: reveived " + version + ", but expected one of " + SPL_MESSAGE_VERSIONS);
        }
    }
}
