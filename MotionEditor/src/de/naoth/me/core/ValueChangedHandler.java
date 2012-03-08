/*
 * 
 */

package de.naoth.me.core;

import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author Heinrich Mellmann
 */

public class ValueChangedHandler<T>
{
  private List<ValueChangedListener<T>> valueChangedListenerList;

    public ValueChangedHandler()
    {
        valueChangedListenerList = new LinkedList<ValueChangedListener<T>>();
    }

    public void addValueChangedListener(ValueChangedListener<T> listener)
    {
        valueChangedListenerList.add(listener);
    }//end addMotionNetChangedListener

    public void removeValueChangedListener(ValueChangedListener<T> listener)
    {
        valueChangedListenerList.remove(listener);
    }//end addMotionNetChangedListener


    protected void performValueChangedEvent(T object)
    {
        for(ValueChangedListener<T> listener: valueChangedListenerList)
        {
            listener.valueChanged(object);
        }//end for
    }//end performMotionNetChangedEvent
}//end ValueChangedHandler
