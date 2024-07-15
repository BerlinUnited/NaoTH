/*
 * The symbols within XABSL can be identified by their name and by 
 * internal id. The id is used for efficiency, e.g., when serializing the values
 * of the symbols can be stored in arrays, where the index is the symbol id.
 * Each of the symbol types is stored in a separate array, e.g., 
 *  double[] decimalSymbolValues;
 *  boolean[] booleanSymbolValues;
 * 
 * SymbolKey creates a unique key for a XABSL symbol based on its type and the id.
 * This allows for the symbols of different types to be stored in the same Map
 * and still keep the information about their ids withing the XABSL arrays.
 * 
 * Example:
 *   
 *   // symbolDefinitions - keeps a list of all symbols defined in the current behavior.
 *   // type: DECIMAL
 *   // id: 0
 *   // Value: 7
 *   Map<SymbolKey, Symbol> symbolDefinitions;
 *   symbolDefinitions.put(new SymbolKey(SymbolType.DECIMAL, 0), Symbol.Decimal("MySymbol", 7));
 *   ...
 *   
 *   // parse values from an array
 *   ArrayList<Symbols> newValues = new ArrayList<>();
 *   for (int id = 0; id < decimalSymbolValues.length; id++) { 
 *     SymbolKey key = new SymbolKey(SymbolType.DECIMAL, id)
 *     // get the Symbol
 *     Symbol symbol = symbolDefinitions.get(key);
 *     // create a new symbol value
 *     Symbol symbolNew = symbol.Decimal(decimalSymbolValues[id]);
 *     // store the new value in the list
 *     newValues.add( symbolNew );
 *   }
 */

package de.naoth.rc.components.behaviorviewer.model;

import java.util.Objects;

/**
 *
 * @author thomas
 */
public class SymbolKey 
{
    private final int id;
    private final SymbolType symbolType;

    public SymbolKey(SymbolType symbolType, int id) {
        this.id = id;
        this.symbolType = symbolType;
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
