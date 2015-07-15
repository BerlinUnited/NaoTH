/*
 */
package de.naoth.rc.dataformats;

import com.google.protobuf.InvalidProtocolBufferException;
import de.naoth.rc.drawings.Circle;
import de.naoth.rc.drawings.DrawingCollection;
import de.naoth.rc.drawings.FillOval;
import de.naoth.rc.drawings.Line;
import de.naoth.rc.drawings.Pen;
import de.naoth.rc.drawings.Robot;
import de.naoth.rc.drawings.Text;
import de.naoth.rc.math.Pose2D;
import de.naoth.rc.math.Vector2D;
import de.naoth.rc.messages.Representations;
import java.awt.Color;
import java.nio.ByteBuffer;

/**
 *
 * @author Heinrich Mellmann
 */
public class SPLMessage {

    public static final int SPL_STANDARD_MESSAGE_STRUCT_VERSION = 6;
    public static final int SPL_STANDARD_MESSAGE_DATA_SIZE = 780;
    public static final int SPL_STANDARD_MESSAGE_SIZE = 70 + SPL_STANDARD_MESSAGE_DATA_SIZE;
    public static final int SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS = 5;

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

    public Representations.BUUserTeamMessage user = null;

    public SPLMessage(Representations.TeamMessage.Data msg) {

        this.averageWalkSpeed = -1;

        this.ballAge = msg.getBallAge() / 1000.0f;

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

    public SPLMessage(ByteBuffer buffer) throws Exception {
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

        try {
            this.user = Representations.BUUserTeamMessage.parseFrom(this.data);
        } catch (InvalidProtocolBufferException ex) {
            // it's not our message
        }
    }
    
    public void draw(DrawingCollection drawings, Color robotColor) {

        // robot
        drawings.add(new Pen(1.0f, robotColor));
        drawings.add(new Robot(pose_x, pose_y, pose_a));

        // striker
        if (intention == 3) {
            drawings.add(new Pen(30, Color.red));
            drawings.add(new Circle((int) pose_x, (int) pose_y, 150));
        }

        // number
        drawings.add(new Pen(1, Color.BLACK));
        drawings.add(new Text((int) pose_x, (int) pose_y + 150, "" + playerNum));

        // ball
        if (ballAge >= 0) {
            drawings.add(new Pen(1, Color.orange));

            Vector2D ballPos = new Vector2D(ball_x, ball_y);
            Pose2D robotPose = new Pose2D(pose_x, pose_y, pose_a);
            Vector2D globalBall = robotPose.multiply(ballPos);
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
        }
    }

}
