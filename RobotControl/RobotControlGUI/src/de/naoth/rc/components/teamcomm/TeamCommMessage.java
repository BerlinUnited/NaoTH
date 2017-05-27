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
         * TeamCommMessage constructor.
         * @param timestamp
         * @param address
         * @param message
         * @param isOpponent 
         */
        public TeamCommMessage(long timestamp, String address, SPLMessage message, boolean isOpponent) {
            this.timestamp = timestamp;
            this.address = address;
            this.message = message;
            this.isOpponent = isOpponent;
        }

        /**
         * Return true if this TeamCommMessage from a opponent.
         * @return true|false
         */
        public boolean isOpponent() {
            return isOpponent;
        }
}
