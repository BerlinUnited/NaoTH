/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import java.lang.reflect.ParameterizedType;
import javax.swing.JPanel;

/**
 * This class is in fact a dialog loader. This is used to wrap 
 * dialog into a plugin.
 * @author Heinrich Mellmann
 * @param <T> 
 */
public class DialogPlugin<T extends AbstractDialog>
    implements Dialog
{
    private T instance = null;

    private Class getInstanceClass()
    {
        return ((Class)((ParameterizedType)this.getClass().
                getGenericSuperclass()).getActualTypeArguments()[0]);
    }

    private T create()
    {
        if(instance == null)
        {
            try {
                instance = (T)(getInstanceClass().newInstance());
                instance.init();
            }catch(Exception ex)
            {
                System.err.println("Could not load the dialog " + getInstanceClass().getSimpleName());
                ex.printStackTrace(System.err);
            }
        }
        return instance;
    }

    @Override
    public String getDisplayName() { 
        return getInstanceClass().getSimpleName(); 
    }

    @Override
    public void dispose() {
        if(instance != null)
        {
            instance.dispose();
        }
    }

    @Override
    public void destroy() {
        dispose();
        instance = null;
    }
    
    
    @Override
    public JPanel getPanel() {
        return create().getPanel();
    }
}//end DialogPlugin