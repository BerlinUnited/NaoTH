package de.naoth.rc.statusbar;

import net.xeoh.plugins.base.Plugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public interface StatusbarPlugin extends Plugin
{
    /**
     * Sets the 'weight' how far on the right side of the statusbar this plugin should be shown.
     * 
     * @return the 'weight'
     */
    public int getWeight();
}
