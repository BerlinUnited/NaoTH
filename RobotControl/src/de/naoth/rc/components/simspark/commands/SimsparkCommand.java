package de.naoth.rc.components.simspark.commands;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public abstract class SimsparkCommand
{
    public abstract String getName();

    public abstract String getCommand();

    @Override
    public String toString()
    {
        return getName();
    }
}
