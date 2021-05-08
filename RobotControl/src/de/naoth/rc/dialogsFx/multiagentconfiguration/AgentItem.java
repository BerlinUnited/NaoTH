package de.naoth.rc.dialogsFx.multiagentconfiguration;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class AgentItem implements Comparable<AgentItem>
{
    private final StringProperty host = new SimpleStringProperty();
    private final IntegerProperty port = new SimpleIntegerProperty(5401);
    private final BooleanProperty active = new SimpleBooleanProperty(true);

    public AgentItem(String host) {
        this.host.set(host);
    }

    public AgentItem(String host, int port) {
        this.host.set(host);
        this.port.set(port);
    }

    public String getHost() { return host.get(); }
    public int getPort() { return port.get(); }
    public boolean getActive() { return active.get(); }
    public StringProperty hostProperty() { return host; }
    public IntegerProperty portProperty() { return port; }
    public BooleanProperty activeProperty() { return active; }

    @Override
    public String toString() {
        return getHost() + ":" + getPort();
    }

    @Override
    public int compareTo(AgentItem o) {
        String a1 = getHost() + ":" + getPort();
        String a2 = o.getHost() + ":" + o.getPort();
        return a1.compareTo(a2);
    }
}
