package de.naoth.rc.statusbar;

import de.naoth.rc.components.gamecontroller.GameControllerEventListener;
import de.naoth.rc.components.gamecontroller.GameControllerManager;
import de.naoth.rc.components.gamecontroller.event.GameControllerConnectionEvent;
import de.naoth.rc.components.gamecontroller.event.GameControllerDataEvent;
import javax.swing.ImageIcon;
import net.xeoh.plugins.base.annotations.PluginImplementation;
import net.xeoh.plugins.base.annotations.injections.InjectPlugin;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */

@PluginImplementation
public class StatusbarGameController extends StatusbarPluginImpl implements GameControllerEventListener
{
    @InjectPlugin
    public static GameControllerManager gamecontroller;
    
    private final ImageIcon iconOff = new ImageIcon(getClass().getResource("/de/naoth/rc/res/con_status_grey20.png"));
    private final ImageIcon iconOn  = new ImageIcon(getClass().getResource("/de/naoth/rc/res/con_status_green20.png"));
    private final ImageIcon iconIm  = new ImageIcon(getClass().getResource("/de/naoth/rc/res/con_status_yellow20.png"));
    
    private final String tooltipOff = "Not connected to GameController";
    private final String tooltipOn  = "Receiving from GameController";
    private final String tooltipIm  = "Listening on GameController (Timeout)";
    private final String tooltipNR  = "Not ready! (Missing GameController library - set via preference dialog)";
    
    @Override
    protected void init() {

        setTooltipHeight(1, 0);
        // initial state
        if(gamecontroller.isReady()) {
            setEnabled(true);
            setIcon(iconOff);
            setToolTipText(tooltipOff);
        } else {
            setEnabled(false);
            setIcon(iconOff);
            setToolTipText(tooltipNR);
        }
        
        gamecontroller.addGameListener(this);

        addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                if(gamecontroller.isReady()) {
                    if(!gamecontroller.isConnected()) {
                        setIcon(iconIm);
                        gamecontroller.connect();
                    } else {
                        setIcon(iconIm);
                        gamecontroller.disconnect();
                    }
                }
            }
        });
    }

    @Override
    protected void exit() {
        gamecontroller.disconnect();
        gamecontroller.removeGameListener(this);
    }

    @Override
    public void newGameData(GameControllerDataEvent e) {
        if(getIcon() != iconOn) {
            setIcon(iconOn);
        }
    }

    @Override
    public void connectionChanged(GameControllerConnectionEvent e) {
        setEnabled(true);
        switch (e.state) {
            case CONNECTED:
                setIcon(iconOn);
                setToolTipText(tooltipOn);
                break;
            case TIMEOUT:
                setIcon(iconIm);
                setToolTipText(tooltipIm);
                break;
            default:
                // DISCONNECTED
                setIcon(iconOff);
                setToolTipText(tooltipOff);
                break;
        }
    }
}
