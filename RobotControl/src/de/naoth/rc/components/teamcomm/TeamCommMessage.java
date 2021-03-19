package de.naoth.rc.components.teamcomm;

import de.naoth.rc.dataformats.SPLMessage;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TeamCommMessage {

    /**
     * The unix timestamp, when the message was received.
     */
    public final long timestamp;

    /**
     * The ip address from which "robot" the message was received.
     */
    public final String address;

    /**
     * The data of the SPL message.
     */
    public final SPLMessage message;

    /**
     * Indicates if this is a message of a teammate or opponent.
     */
    private final boolean isOpponent;

    /**
     * Represents the game state, when this message was sent. '-1' is unknown.
     */
    public final int gameState;

    /**
     * Constructor, initializes this representation.
     *
     * @param timestamp
     * @param address
     * @param message
     * @param isOpponent
     */
    public TeamCommMessage(long timestamp, String address, SPLMessage message, boolean isOpponent) {
        this(timestamp, address, message, isOpponent, -1);
    }

    /**
     * Constructor, initializes this representation.
     *
     * @param timestamp
     * @param address
     * @param message
     * @param isOpponent
     * @param gameState
     */
    public TeamCommMessage(long timestamp, String address, SPLMessage message, boolean isOpponent, int gameState) {
        this.timestamp = timestamp;
        this.address = address;
        this.message = message;
        this.isOpponent = isOpponent;
        this.gameState = gameState;
    }

    /**
     * Return true if this TeamCommMessage from a opponent.
     *
     * @return true|false
     */
    public boolean isOpponent() {
        return isOpponent;
    }
}
