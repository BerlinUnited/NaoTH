package de.naoth.rc.dialogsFx.moduleconfiguration;

import java.util.ArrayList;
import java.util.List;

/**
 * The representation type class. Represents a representation with its
 * dependencies (modules).
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Representation implements Type, Comparable<Representation>
{
    /**
     * The possible representation "info state" based on the module
     * configuration (see getInfo())
     */
    public static enum Info {
        EMPTY, // not provided and not required
        INFO, //     provided and     required
        WARNING, //     provided but not required
        ERROR    // not provided but     required
    }

    /**
     * Name of the representation
     */
    private final String name;

    /**
     * The requiring modules
     */
    private final List<Type> required = new ArrayList<>();
    /**
     * The providing modules
     */
    private final List<Type> provided = new ArrayList<>();

    /**
     * Constructor. Creates the representation with the given name.
     *
     * @param name of the representation
     */
    public Representation(String name) {
        this.name = name;
    }

    /**
     * Returns just the name of this representation
     *
     * @return name of the representation
     */
    @Override
    public String toString() {
        return name;
    }

    /**
     * Adds the providing module to this representation.
     *
     * @param m the providing module
     */
    public void addProvider(Module m) {
        provided.add(m);
    }

    /**
     * Adds the module which requires this representation.
     *
     * @param m the module requiring this representation
     */
    public void addRequirer(Module m) {
        required.add(m);
    }

    /**
     * Compares this representation with another based on their names (case
     * insensitive). Used to sort the list of representations.
     *
     * @param o the other module
     * @return result of the comparison: -1, 0, 1
     */
    @Override
    public int compareTo(Representation o) {
        return name.compareToIgnoreCase(o.name);
    }

    /**
     * Returns the name of this representation.
     *
     * @return the representations name
     */
    @Override
    public String getName() {
        return name;
    }

    /**
     * Returns the list of modules requiring this representation.
     *
     * @return module list
     */
    @Override
    public List<Type> getRequire() {
        return required;
    }

    /**
     * Returns the list of modules providing this representation.
     *
     * @return module list
     */
    @Override
    public List<Type> getProvide() {
        return provided;
    }

    /**
     * Returns the "info state" of the representation. The "info state"
     * determines, whether the representation is used correctly with the current
     * active module configuration: If the representation is - neither provided
     * nor required: EMPTY state - provided but not required: WARNING state -
     * not provided but required: ERROR state - provided and required: INFO
     * state
     *
     * @return the current state
     */
    public Info getInfo() {
        // check if there's an active providing/requiring module
        boolean isProvided = provided.stream().anyMatch((m) -> ((Module) m).isActive());
        boolean isRequired = required.stream().anyMatch((m) -> ((Module) m).isActive());

        if (provided.isEmpty() && required.isEmpty()) {
            return Info.EMPTY;   // not provided and not required
        } else if (isProvided && !isRequired) {
            return Info.WARNING; //     provided but not required
        } else if (!isProvided && isRequired) {
            return Info.ERROR;   // not provided but     required
        }

        return Info.INFO;        //     provided and     required
    }
}
