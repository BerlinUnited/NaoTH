/*
 */

package naoscp.tools;

import java.awt.Component;
import java.awt.Container;

/**
 *
 * @author Heinrich Mellmann
 */
public class SwingTools 
{
    public static void setEnabled(Component component, boolean enabled) {
        component.setEnabled(enabled);
        if (component instanceof Container) {
            for (Component child : ((Container) component).getComponents()) {
                setEnabled(child, enabled);
            }
        }
    }
}
