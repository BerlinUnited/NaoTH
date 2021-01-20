package de.naoth.rc.dialogsFx.moduleconfiguration;

import java.util.ArrayList;
import java.util.List;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;

/**
 * The module type class. Represents a module with its dependencies
 * (representations).
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Module implements Type, Comparable<Module>
{
    /**
     * The type of moulde (cognition/motion)
     */
    private final String type;
    /**
     * The modules name
     */
    private final String name;
    /**
     * The path, where the module is defined in C++
     */
    private final String path;
    /**
     * The module state flag: enabled or disabled
     */
    private final BooleanProperty active = new SimpleBooleanProperty(false);
    /**
     * The requiring representations
     */
    private final List<Type> require = new ArrayList<>();
    /**
     * The providing representations
     */
    private final List<Type> provide = new ArrayList<>();

    /**
     * Constructor - initialise the module.
     *
     * @param type module type (cognition/motion)
     * @param name module name
     * @param path module path (C++)
     * @param active enabled/disabled flag
     */
    public Module(String type, String name, String path, boolean active) {
        this.type = type;
        this.name = name;
        this.path = path;
        this.active.set(active);
    }

    /**
     * Returns just the name of this module
     *
     * @return name of the module
     */
    @Override
    public String toString() {
        return name;
    }

    /**
     * Adds the representation as required to this module.
     *
     * @param r the required representation
     */
    public void addRequire(Representation r) {
        require.add(r);
    }

    /**
     * Adds the representation as provided to this module.
     *
     * @param r the providing representation
     */
    public void addProvide(Representation r) {
        provide.add(r);
    }

    /**
     * Compares this module with another based on their names (case
     * insensitive). Used to sort the list of modules.
     *
     * @param o the other module
     * @return result of the comparison: -1, 0, 1
     */
    @Override
    public int compareTo(Module o) {
        return name.compareToIgnoreCase(o.name);
    }

    /**
     * Returns the type of this module (cognition/motion).
     *
     * @return modules type
     */
    public String getType() {
        return type;
    }

    /**
     * Returns the name of this module.
     *
     * @return modules name
     */
    @Override
    public String getName() {
        return name;
    }

    /**
     * Returns the path of this module.
     *
     * @return modules path
     */
    public String getPath() {
        return path;
    }

    /**
     * Returns the list of required types (representations)
     *
     * @return required representations
     */
    @Override
    public List<Type> getRequire() {
        return require;
    }

    /**
     * Returns the list of provided types (representations)
     *
     * @return provided representations
     */
    @Override
    public List<Type> getProvide() {
        return provide;
    }
    
    /**
     * Returns the active property flag.
     * 
     * @return the active property
     */
    public BooleanProperty getActiveProperty() {
        return active;
    }

    /**
     * Returns whether this module is enabled or not
     *
     * @return true, if enabled, false otherwise
     */
    public boolean isActive() {
        return active.get();
    }
}
