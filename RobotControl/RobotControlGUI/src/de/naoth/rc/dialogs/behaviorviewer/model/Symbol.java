/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs.behaviorviewer.model;

import de.naoth.rc.dialogs.behaviorviewer.XABSLBehavior;

/**
 *
 * @author thomas
 */
public class Symbol {
    public final String name;

    public Symbol(String name) {
        this.name = name;
    }

    public String getValueAsString() {
        return "";
    }

    public SymbolType getDataType() {
        return SymbolType.VOID;
    }

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

        @Override
        public String getValueAsString() {
            return "" + this.value;
        }

        @Override
        public SymbolType getDataType() {
            return SymbolType.DECIMAL;
        }
    }

    public static class Boolean extends Symbol {

        public Boolean(String name, boolean value) {
            super(name);
            this.value = value;
        }
        public final boolean value;

        @Override
        public String getValueAsString() {
            return "" + this.value;
        }

        @Override
        public SymbolType getDataType() {
            return SymbolType.BOOL;
        }
    }

    public static class Enum extends Symbol {

        public Enum(String name, EnumType enumType, int value) {
            super(name);
            this.enumType = enumType;
            this.value = value;
        }
        public final int value;
        public final EnumType enumType;

        @Override
        public String getValueAsString() {
            return enumType.elements.get(value).name;
        }

        @Override
        public SymbolType getDataType() {
            return SymbolType.ENUM;
        }
    }
    
} // end class Symbol
