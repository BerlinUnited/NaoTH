package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AckCommand extends SimsparkCommand
{
    @Override
    public String getName()
    {
        return "Ack";
    }

    @Override
    public String getCommand()
    {
        return "(getAck)";
    }
}
