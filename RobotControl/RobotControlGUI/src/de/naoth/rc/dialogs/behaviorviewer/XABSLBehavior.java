/*
 * 
 */
package de.naoth.rc.dialogs.behaviorviewer;

import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLBehavior {

    public static class Symbol {
        public Symbol(String name) { this.name = name; }
        public final String name;
    
        public Decimal createDecimal(double value) {
            return new Decimal(name, value);
        }
        public Boolean createBoolean(boolean value) {
            return new Boolean(name, value);
        }
        public Enum createEnum(EnumType enumType, int value) {
            return new Enum(name, enumType, value);
        }
        
        public static class Decimal extends Symbol {
            public Decimal(String name, double value) {
                super(name);
                this.value = value;
            }
            public final double value;
        }

        public static class Boolean extends Symbol {

            public Boolean(String name, boolean value) {
                super(name);
                this.value = value;
            }
            public final boolean value;
        }

        public static class Enum extends Symbol {

            public Enum(String name, EnumType enumType, int value) {
                super(name);
                this.enumType = enumType;
                this.value = value;
            }
            public final int value;
            public final EnumType enumType;
        }
    }

    public static class EnumType {

        public static class Element {

            public String name;
            public int value;
        }
        public String name;
        public ArrayList<Element> elements = new ArrayList<Element>();
    }

    public static class Option {

        public static class State {
            State(String name, boolean target)
            {
                this.name = name;
                this.target = target;
            }
            public final String name;
            public final boolean target;
        }
        
        public Option(String name) {
            this.name = name;
        }
        
        public Option(Option other) {
            name = other.name;
            states.addAll(other.states);
            parameters.addAll(other.parameters);
        }
        
        public final String name;
        public final ArrayList<State> states = new ArrayList<State>();
        public final ArrayList<Symbol> parameters = new ArrayList<Symbol>();
    }

    public static class Agent {

        public String name;
        public String rootOption;
    }

    public ArrayList<EnumType> enumerations = new ArrayList<EnumType>();
    public ArrayList<Option> options = new ArrayList<Option>();
    public ArrayList<Agent> agents = new ArrayList<Agent>();
    
    public ArrayList<Symbol> inputSymbols = new ArrayList<Symbol>();
    public ArrayList<Symbol> outputSymbols = new ArrayList<Symbol>();
}
