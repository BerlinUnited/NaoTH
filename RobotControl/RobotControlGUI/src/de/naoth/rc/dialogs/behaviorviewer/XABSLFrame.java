/*
 * 
 */
package de.naoth.rc.dialogs.behaviorviewer;

import de.naoth.rc.dialogs.behaviorviewer.XABSLFramePrototype.SymbolId;
import de.naoth.rc.messages.Messages;
import java.util.ArrayList;

/**
 *
 * @author Heinrich Mellmann
 */
public class XABSLFrame {

    public XABSLFrame(XABSLFramePrototype proto, Messages.BehaviorStatus status) throws Exception {
        this.inputSymbolValuesDecimal = new double[proto.numberOfInputSymbolsDecimal];
        this.inputSymbolValuesBoolean = new boolean[proto.numberOfInputSymbolsBoolean];
        this.inputSymbolValuesEnum = new String[proto.numberOfInputSymbolsEnum];

        this.outputSymbolValuesDecimal = new double[proto.numberOfOutputSymbolsDecimal];
        this.outputSymbolValuesBoolean = new boolean[proto.numberOfOutputSymbolsBoolean];
        this.outputSymbolValuesEnum = new String[proto.numberOfOutputSymbolsEnum];

        this.activeActions = new ArrayList<Messages.XABSLAction>();

        this.frameNumber = status.getFrameNumber();

        for (Messages.XABSLParameter p : status.getInputSymbolsList()) {
            int idx = proto.symbolRegistry.get(p.getName()).idx;
            if (idx == -1) {
                throw new Exception("wrong idx for symbol " + p.getName());
            }

            if (p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue()) {
                inputSymbolValuesDecimal[idx] = p.getDecimalValue();
            } else if (p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue()) {
                inputSymbolValuesBoolean[idx] = p.getBoolValue();
            } else if (p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue()) {
                inputSymbolValuesEnum[idx] = p.getEnumValue();
            }
        }//end for

        for (Messages.XABSLParameter p : status.getOutputSymbolsList()) {
            int idx = proto.symbolRegistry.get(p.getName()).idx;
            if (idx == -1) {
                throw new Exception("wrong idx for symbol " + p.getName());
            }

            if (p.getType() == Messages.XABSLParameter.ParamType.Decimal && p.hasDecimalValue()) {
                outputSymbolValuesDecimal[idx] = p.getDecimalValue();
            } else if (p.getType() == Messages.XABSLParameter.ParamType.Boolean && p.hasBoolValue()) {
                outputSymbolValuesBoolean[idx] = p.getBoolValue();
            } else if (p.getType() == Messages.XABSLParameter.ParamType.Enum && p.hasEnumValue()) {
                outputSymbolValuesEnum[idx] = p.getEnumValue();
            }
        }//end for

        for (Messages.XABSLAction a : status.getActiveRootActionsList()) {
            this.activeActions.add(a);
        }

        this.agent = status.getAgent();
    }//end constructor

    public final double[] inputSymbolValuesDecimal;
    public final boolean[] inputSymbolValuesBoolean;
    public final String[] inputSymbolValuesEnum;

    public final double[] outputSymbolValuesDecimal;
    public final boolean[] outputSymbolValuesBoolean;
    public final String[] outputSymbolValuesEnum;

    public final ArrayList<Messages.XABSLAction> activeActions;
    public final String agent;

    public final int frameNumber;

    public String getStringValue(SymbolId id) {
        if (id.io_type == SymbolId.IOType.input) {
            switch (id.data_type) {
                case Decimal:
                    return "" + inputSymbolValuesDecimal[id.idx];
                case Boolean:
                    return "" + inputSymbolValuesBoolean[id.idx];
                case Enum:
                    return "" + inputSymbolValuesEnum[id.idx];
                default:
                    return "";
            }//end switch
        } else {
            switch (id.data_type) {
                case Decimal:
                    return "" + outputSymbolValuesDecimal[id.idx];
                case Boolean:
                    return "" + outputSymbolValuesBoolean[id.idx];
                case Enum:
                    return "" + outputSymbolValuesEnum[id.idx];
                default:
                    return "";
            }//end switch
        }
    }//end getStringValue
}//end class XABSLFrame
