package de.naoth.rc.dataformats;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.io.StreamTokenizer;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Iterator;

/**
 *
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class Sexp {
    SexpTokenizer t;

    public Sexp() {
    }
    
    public Sexp(String expr) {
        setExpression(expr);
    }
    
    public ArrayList<Object> parseSexp() {
        ArrayList<Object> m = new ArrayList<>();
        while(t.hasNext()) {
            m.add(parseSexpAttribute());
        }
        return m;
    }
    
    private Object parseSexpAttribute() {        
        SexpToken token = t.next();
        switch(token.type)
        {
            case '(':
                return parseSexpList(token);
            default:
                return token.text;
        }
    }
    
    private ArrayList<Object> parseSexpList(SexpToken token_open) {
        ArrayList<Object> currentValue = new ArrayList<>();
        
        while(t.peekToken().type != ')') {
            Object o = parseSexpAttribute();
            if(o != null) {
                currentValue.add(o);
            }
        }
        SexpToken token_close = t.next();
        
        if(currentValue.isEmpty()) {
            return null;
        }
        
        return currentValue;
    }
    
    public void setExpression(String expr) {
        t = new SexpTokenizer(expr);
    }
}

class SexpTokenizer implements Iterator<SexpToken> {
    // Instance variables have default access to allow unit tests access.
    StreamTokenizer m_tokenizer;
    IOException m_ioexn;
 
    /** Constructs a tokenizer that scans input from the given string.
     * @param src A string containing S-expressions.
     */
    public SexpTokenizer(String src)
    {
        this(new StringReader(src));
    }
 
    /** Constructs a tokenizer that scans input from the given Reader.
     * @param r Reader for the character input source
     */
    public SexpTokenizer(Reader r)
    {
        if(r == null)
            r = new StringReader("");
        BufferedReader buffrdr = new BufferedReader(r);
        m_tokenizer = new StreamTokenizer(buffrdr);
        m_tokenizer.resetSyntax(); // We don't like the default settings
 
        m_tokenizer.whitespaceChars(0, ' ');
        m_tokenizer.wordChars(' '+1,255);
        m_tokenizer.ordinaryChar('(');
        m_tokenizer.ordinaryChar(')');
        m_tokenizer.ordinaryChar('\'');
        m_tokenizer.commentChar(';');
        m_tokenizer.quoteChar('"');
    }
 
    public SexpToken peekToken()
    {	
        if(m_ioexn != null)
            return null;
        try
        {
            m_tokenizer.nextToken();
        }
        catch(IOException e)
        {
            m_ioexn = e;
            return null;
        }
        if(m_tokenizer.ttype == StreamTokenizer.TT_EOF)
            return null;
        SexpToken token = new SexpToken(m_tokenizer);
        m_tokenizer.pushBack();
        return token;
    }
 
    public boolean hasNext()
    {
        if(m_ioexn != null)
            return false;
        try
        {
            m_tokenizer.nextToken();
        }
        catch(IOException e)
        {
            m_ioexn = e;
            return false;
        }
        if(m_tokenizer.ttype == StreamTokenizer.TT_EOF)
            return false;
        m_tokenizer.pushBack();
        return true;
    }
 
    /** Return the most recently caught IOException, if any,
     * 
     * @return
     */
    public IOException getIOException()
    {
        return m_ioexn;
    }
 
    public SexpToken next()
    {
        try
        {
            m_tokenizer.nextToken();
        }
        catch(IOException e)
        {
            m_ioexn = e;
            return null;
        }
 
        SexpToken token = new SexpToken(m_tokenizer);
        return token;
    }
 
    public void remove()
    {
    }
}
