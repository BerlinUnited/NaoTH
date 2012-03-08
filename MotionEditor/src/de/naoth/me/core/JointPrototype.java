/*
 * 
 */

package de.naoth.me.core;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/**
 *
 * @author Heinrich Mellmann
 */
public class JointPrototype
{
    private final String id;
    private int minValue;
    private int maxValue;
    private int defaultValue;
    private boolean valid;

    public JointPrototype (String id, double minValue, double maxValue, double defaultValue)
    {
        this.id = id;
        this.minValue = (int)minValue;
        this.maxValue = (int)maxValue;
        this.defaultValue = (int)defaultValue;
        this.valid = false;
    }//end JointValue

    public String getId()
    {
        return this.id;
    }
    
    public int getMinValue()
    {
        return this.minValue;
    }

    public int getMaxValue()
    {
        return this.maxValue;
    }

    public int getDefaultValue()
    {
        return this.defaultValue;
    }

    public boolean isValid()
    {
        return this.valid;
    }

    public void setValid(boolean valid)
    {
      this.valid = valid;
    }


}//end class JointValuePrototype
