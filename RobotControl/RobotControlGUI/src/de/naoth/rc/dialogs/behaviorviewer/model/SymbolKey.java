/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs.behaviorviewer.model;

import de.naoth.rc.messages.Messages;
import java.util.Objects;

/**
 *
 * @author thomas
 */
public class SymbolKey {
    private final int id;
    private final SymbolType symbolType;

    public SymbolKey(SymbolType dateType, int id) {
        this.id = id;
        this.symbolType = dateType;
    }
    
    public SymbolKey(Messages.XABSLSymbol.SymbolType symbolType, int id) {
        this.id = id;
        this.symbolType = SymbolType.fromMessageType(symbolType);
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 83 * hash + this.id;
        hash = 83 * hash + Objects.hashCode(this.symbolType);
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final SymbolKey other = (SymbolKey) obj;
        if (this.id != other.id) {
            return false;
        }
        if (this.symbolType != other.symbolType) {
            return false;
        }
        return true;
    }

    public int getId() {
        return id;
    }

    public SymbolType getSymbolType() {
        return symbolType;
    }
    
}
