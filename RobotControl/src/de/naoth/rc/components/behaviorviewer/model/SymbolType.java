/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.components.behaviorviewer.model;

/**
 *
 * @author thomas
 */
public class SymbolType {
    public final static SymbolType VOID = new SymbolType("");
    public final static SymbolType DECIMAL = new SymbolType("decimal");
    public final static SymbolType BOOL = new SymbolType("bool");
    public final static SymbolType ENUM = new SymbolType("enum");
    
    public final String name;
    
    SymbolType(String name)
    {
        this.name = name;
    }

    @Override
    public String toString() {
        return name;
    }   
}
