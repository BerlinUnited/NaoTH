/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.components.behaviorviewer.model;

import java.util.Map;
import java.util.TreeMap;

/**
 *
 * @author thomas
 */
public class EnumType extends SymbolType{

    public static class Element {
        public final int value;
        public final String name;
        
        public Element(int value, String name) {
            this.value = value;
            this.name = name;
        }
    }
    
    //public final String name;
    public final Map<Integer, Element> elements = new TreeMap<>();

    public EnumType(String name)
    {
        super(name);
    }
    
    @Override
    public String toString() {
        return "enum " + this.name;
    }
}
