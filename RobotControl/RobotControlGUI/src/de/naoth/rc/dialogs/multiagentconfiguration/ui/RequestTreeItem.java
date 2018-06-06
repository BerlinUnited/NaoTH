package de.naoth.rc.dialogs.multiagentconfiguration.ui;

import javafx.scene.control.CheckBoxTreeItem;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RequestTreeItem extends CheckBoxTreeItem<String>
{
    private String request;
    
    public RequestTreeItem(String value) {
        super(value);
    }
    
    public void setRequest(String request) {
        this.request = request;
    }
    
    public String getRequest() {
        return request;
    }
}
