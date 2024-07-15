package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class DropBallCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "DropBall";
    }

    @Override
    public String getCommand()
    {
        return "(dropBall)";
    }
}
