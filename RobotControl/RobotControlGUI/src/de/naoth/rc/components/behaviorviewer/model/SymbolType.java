/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.components.behaviorviewer.model;

import de.naoth.rc.messages.Messages;

/**
 *
 * @author thomas
 */
public enum SymbolType {
    VOID(""), DECIMAL("decimal"), BOOL("bool"), ENUM("enum");
    
    private final String humanReadable;
    
    SymbolType(String humanReadable)
    {
        this.humanReadable = humanReadable;
    }

    @Override
    public String toString() {
        return humanReadable;
    }
    
    
    public static SymbolType fromMessageType(Messages.XABSLSymbol.SymbolType type) {
        switch(type)
        {
            case Boolean:
                return BOOL;
            case Enum:
                return ENUM;
            case Decimal:
                return DECIMAL;
            default:
                return VOID;
        }
    }
    
}
