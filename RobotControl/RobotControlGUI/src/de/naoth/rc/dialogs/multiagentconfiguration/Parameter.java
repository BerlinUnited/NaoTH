package de.naoth.rc.dialogs.multiagentconfiguration;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Parameter
{
    private StringProperty name;
    private StringProperty value;

    public Parameter(String name, String value) {
        setName(name);
        setValue(value);
    }

    public void setName(String value) {
        nameProperty().set(value);
    }

    public String getName() {
        return nameProperty().get();
    }

    public StringProperty nameProperty() {
        if (name == null) {
            name = new SimpleStringProperty(this, "name");
        }
        return name;
    }

    public void setValue(String value) {
        valueProperty().set(value);
    }

    public String getValue() {
        return valueProperty().get();
    }

    public StringProperty valueProperty() {
        if (value == null) {
            value = new SimpleStringProperty(this, "value");
        }
        return value;
    }
}
