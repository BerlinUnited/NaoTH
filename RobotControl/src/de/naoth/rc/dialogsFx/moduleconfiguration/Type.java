package de.naoth.rc.dialogsFx.moduleconfiguration;

import java.util.List;

/**
 * Base class for module/representation
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface Type
{
    /**
     * Return the name of this type.
     * 
     * @return name of the type
     */
    public String getName();

    /**
     * Returns the required types by this type.
     * 
     * @return list of requirements
     */
    public List<Type> getRequire();

    /**
     * Returns the provided types by this type.
     * 
     * 
     * @return list of providing types
     */
    public List<Type> getProvide();
}
