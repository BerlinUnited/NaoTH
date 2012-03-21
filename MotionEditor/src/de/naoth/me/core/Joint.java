package de.naoth.me.core;



/**
 *
 * @author Heinrich Mellmann
 */
public class Joint extends ValueChangedHandler<Joint>
{
    private double value;
    private JointPrototype prototype;
    private boolean valid;
   
    public Joint(JointPrototype prototype)
    {
        this.prototype = prototype;
        this.value = prototype.getDefaultValue();
        this.valid = prototype.isValid();
    }//end JointValue
    
    
    public String getId()
    {
        return this.prototype.getId();
    }
    
    public double getMinValue()
    {
        return this.prototype.getMinValue();
    }
    
    public double getMaxValue()
    {
        return this.prototype.getMaxValue();
    }
    
    public double getDefaultValue()
    {
        return this.prototype.getDefaultValue();
    }
    
    /* Change: Sven Lund
     * Semantic should be: value > max -> value = max
     *                     value < min -> value = min
     *                     if value is set it is valid         
     */
    public void setValue(double value)
    {
        //if(value <= this.prototype.maxValue && value >= this.prototype.minValue)
        //{
        //    this.value = value;
        //}
        this.value = value;
        if (value > prototype.getMaxValue()) value = prototype.getMaxValue();
        if (value < prototype.getMinValue()) value = prototype.getMinValue();
        performValueChangedEvent(this);
    }//end setValue

    public void setDegrees(double value)
    {
        setValue(value);
        performValueChangedEvent(this);
    }//end setDegrees

    public void setRadiant(double value)
    {
        setValue(value * 180.0 / Math.PI);
        performValueChangedEvent(this);
    }//end setDegrees

    // returns the value in radiant
    public double getRadiant()
    {
        return value * Math.PI / 180.0;
    }
    
    public double getValue()
    {
        return value;
    }
    
    public boolean isValid()
    {
        return this.prototype.isValid();
    }

    public void setValid(boolean valid)
    {
      this.prototype.setValid(valid);
    }

    // I think we don'T need this Method because of the new semantic of the setValue(double value)
    //public void setValid(boolean valid) {
    //    this.valid = valid;
    //}
}//end class JointValue
