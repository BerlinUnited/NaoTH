package de.naoth.rc.dialogsFx.multiagentconfiguration;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Parameter
{
    private StringProperty type;
    private StringProperty module;
    private StringProperty name;
    private StringProperty value;

    public Parameter(String name, String value) {
        setName(name);
        setValue(value);
    }
    
    public Parameter(String type, String module, String name, String value) {
        setType(type);
        setModule(module);
        setName(name);
        setValue(value);
    }
    
    public void setType(String type) {
        typeProperty().set(type);
    }
    
    public String getType() {
        return typeProperty().get();
    }
    
    public StringProperty typeProperty() {
        if(type == null) {
            type = new SimpleStringProperty(this, "type");
        }
        return type;
    }

    public void setModule(String clazz) {
        moduleProperty().set(clazz);
    }
    
    public String getModule() {
        return moduleProperty().get();
    }
    
    public StringProperty moduleProperty() {
        if(module == null) {
            module = new SimpleStringProperty(this, "module");
        }
        return module;
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

    @Override
    public String toString() {
        return "Parameter (" + (type==null?"null":type.get()) + ":" + (module==null?"null":module.get()) + "." + name.get() + "=" + value.get() + ")";
    }
}
