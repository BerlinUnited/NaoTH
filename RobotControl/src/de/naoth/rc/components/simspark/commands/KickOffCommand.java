package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class KickOffCommand extends SimsparkCommand
{
    final private String team;

    public KickOffCommand(String team) throws IllegalArgumentException
    {
        if (team != null && !team.equals("Left") && !team.equals("Right"))
        {
            throw new IllegalArgumentException("Only null, 'Left' and 'Right' are allowed!");
        }

        this.team = team;
    }

    public static KickOffCommand kickOffLeft()
    {
        return new KickOffCommand("Left");
    }

    public static KickOffCommand kickOffRight()
    {
        return new KickOffCommand("Right");
    }

    @Override
    public String getName()
    {
        return "KickOff";
    }

    @Override
    public String getCommand()
    {
        return "(kickOff " + (team == null ? "None" : team) + ")";
    }
}
