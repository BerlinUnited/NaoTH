package de.naoth.rc.dialogsFx.multiagentconfiguration.ui;

import javafx.beans.property.StringProperty;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RepresentationsContent
{
    @FXML
    private Label name;
    
    @FXML
    protected TextArea content;
    
    @FXML
    private void initialize() {
        content.setEditable(false);
    }
    
    public void setName(String n) {
        if(name != null) {
            name.setText(n);
        }
    }
    
    public StringProperty getTextProperty() {
        if(content != null) {
            return content.textProperty();
        }
        return null;
    }
}
