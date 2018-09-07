package de.naoth.rc.components.gamecontroller;

/**
 * Just a wrapper for the GameControlData.
 * To reduce the dependency and have a uniform 'interface'.
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class GameControllerData
{
    public byte playersPerTeam;
    public byte competitionType;
    public byte gamePhase;
    public byte gameState;
    public byte firstHalf;
    public short secsRemaining;
    public short secondaryTime;
    
    @Override
    public String toString() {
        return "competitionType = " + competitionType + "\n"
            + "firstHalf = " + firstHalf + "\n"
            + "playersPerTeam = " + playersPerTeam + "\n"
            + "gamePhase = " + gamePhase + "\n"
            + "gameState = " + gameState + "\n"
            + "secsRemaining = " + secsRemaining + "\n"
            + "secondaryTime = " + secondaryTime + "\n"
            ;
    }
}
