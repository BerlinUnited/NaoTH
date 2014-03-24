/*
 * 
 */
package de.naoth.rc.dialogs.behaviorviewer;

import java.util.HashMap;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLFramePrototype {

    public static class SymbolId {

        public enum IOType {

            input, output
        }

        public enum DataType {

            Decimal, Boolean, Enum
        }

        public SymbolId(int idx, DataType data_type, IOType io_type) {
            this.idx = idx;
            this.data_type = data_type;
            this.io_type = io_type;
        }

        public final int idx;
        public final DataType data_type;
        public final IOType io_type;
    }//end class SymbolId

    public int numberOfInputSymbolsDecimal = 0;
    public int numberOfInputSymbolsBoolean = 0;
    public int numberOfInputSymbolsEnum = 0;

    public int numberOfOutputSymbolsDecimal = 0;
    public int numberOfOutputSymbolsBoolean = 0;
    public int numberOfOutputSymbolsEnum = 0;

    public HashMap<String, SymbolId> symbolRegistry = new HashMap<String, SymbolId>();
}//end class XABSLFramePrototype
