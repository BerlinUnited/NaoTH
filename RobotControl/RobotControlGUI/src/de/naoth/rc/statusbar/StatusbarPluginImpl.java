package de.naoth.rc.statusbar;

import de.naoth.rc.RobotControl;
import java.awt.FontMetrics;
import java.awt.Point;
import java.awt.event.MouseEvent;
import javax.swing.JLabel;
import javax.swing.UIManager;
import net.xeoh.plugins.base.annotations.events.PluginLoaded;
import net.xeoh.plugins.base.annotations.events.Shutdown;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public abstract class StatusbarPluginImpl extends JLabel implements StatusbarPlugin
{
    /** The (parent) RC plugin. */
    @InjectPlugin
    public static RobotControl rc;
    /** The application tooltip height. */
    private final FontMetrics metrics;
    /** Caches the calculated plugins tooltip height. */
    private int tooltipHeight;
    
    /**
     * Constructor.
     * Tries to determine the tooltip font, in order to calculate the correct tooltip height.
     * The font height is necessary to calculate the correct Tooltip height!
     */
    public StatusbarPluginImpl() {
        metrics = this.getFontMetrics(UIManager.getLookAndFeelDefaults().getFont("ToolTip.font"));
    }
    
    /**
     * Gets called, when the RC plugin was loaded.
     * Adds the icon/label to the RC statusbar and registers itself as connection listener.
     * 
     * @param robotControl 
     */
    @PluginLoaded
    public void loaded(RobotControl robotControl) {
        init();
        rc.addToStatusBar(this);
    }
    
    /**
     * Gets called, when the application exits and calls the plugins 'exit()' method.
     */
    @Shutdown
    public void shutdown() {
        exit();
    }
    
    /**
     * Calculates the tooltip height based on the number of lines in the tooltip and the spacing between each line.
     * 
     * @param lines number of line in the tooltip
     * @param spacing spacing between lines
     */
    protected void setTooltipHeight(int lines, int spacing) {
        // calculate tooltip height: line-height * line-num + spacing-height * spacing-num + statusbar-height
        tooltipHeight = metrics.getHeight() * lines + spacing * lines + 25;
    }

    /**
     * Returns the location of the tooltip.
     * 
     * @param event
     * @return point, where the tooltip should be shown
     */
    @Override
    public Point getToolTipLocation(MouseEvent event) {
        return new Point(0, -tooltipHeight);
    }
    
    /**
     * Sets the 'weight' how far on the right side of the statusbar this plugin should be shown.
     * 
     * @return the 'weight'
     */
    @Override
    public int getWeight() {
        return 0;
    }
    
    /**
     * This method is called, when the plugin was loaded and before it is added to the statusbar.
     */
    abstract protected void init();
    
    /**
     * Gets called when the application exits.
     */
    abstract protected void exit();
}
