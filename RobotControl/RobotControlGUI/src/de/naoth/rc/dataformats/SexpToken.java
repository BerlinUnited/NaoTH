/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.dataformats;

import java.io.StreamTokenizer;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SexpToken
{
    public static final int SYMBOL = StreamTokenizer.TT_WORD;
    public int type;
    public String text;
    public int line;
 
    public SexpToken(StreamTokenizer tzr)
    {
        this.type = tzr.ttype;
        this.text = tzr.sval;
        this.line = tzr.lineno();
    }
 
    public String toString()
    {
        switch(this.type)
        {
            case SYMBOL:
            case '"':
                return this.text;
            default:
                return String.valueOf((char)this.type);
        }
    }
}