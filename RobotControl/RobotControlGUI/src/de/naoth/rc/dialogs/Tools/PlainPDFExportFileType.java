/*
 * 
 */
package de.naoth.rc.dialogs.Tools;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.util.Map;
import java.util.Properties;
import org.freehep.graphics2d.VectorGraphics;
import org.freehep.graphicsio.PageConstants;
import org.freehep.graphicsio.pdf.PDFGraphics2D;
import org.freehep.util.UserProperties;
import org.freehep.util.export.ExportFileType;

/**
 *
 * @author Heinrich Mellmann
 */
public class PlainPDFExportFileType extends ExportFileType
{
    
    // --- EVIL HACK (set custom size)---
    // there is no possibility to set a custom size
    // properly. The only way is to modify the 
    // private static field PageConstants.sizeTable
    private void setSize(Dimension d)
    {
        try
        {
            // add the custom dimensions to the static map
            Field sizeTableField = PageConstants.class.getDeclaredField("sizeTable");
            sizeTableField.setAccessible(true);
            Map sizeTable = (Map)sizeTableField.get(null);
            sizeTable.put(PageConstants.CUSTOM_PAGE_SIZE, d);
            sizeTableField.setAccessible(false);
        }
        catch( NoSuchFieldException es){}
        catch (IllegalAccessException ill){}

        // set the cusom page size as default option 
        UserProperties p = (UserProperties)PDFGraphics2D.getDefaultProperties();
        p.setProperty(PDFGraphics2D.PAGE_SIZE, PageConstants.CUSTOM_PAGE_SIZE);
    }//end setSize
            
    
    // --- ANOTHER EVIL HACK (remove margins) ---
    static
    {
        UserProperties p = (UserProperties)PDFGraphics2D.getDefaultProperties();
        p.setProperty(PDFGraphics2D.PAGE_MARGINS, new Insets(0, 0, 0, 0));
    }
    
    
    public String getDescription() {
		return "Portable Document Format";
	}

	public String[] getExtensions() {
		return new String[] { "pdf" };
	}

	public String[] getMIMETypes() {
		return new String[] { "application/pdf" };
	}
        
        @Override
    public void exportToFile(OutputStream out, Component[] cmpnts, Component exportDlg, Properties prprts, String creator) throws IOException {
        for(int i = 0; i < cmpnts.length; i++)
        {
            Component c = cmpnts[i];
            
            // --- EVIL HACK ---
            setSize(c.getSize());
            
            VectorGraphics graphics = new PDFGraphics2D(out, c);
            graphics.startExport();
            c.print(graphics);
            graphics.endExport();
        }
    }

    @Override
    public void exportToFile(File file, Component[] cmpnts, Component exportDlg, Properties prprts, String string) throws IOException {
        for(int i = 0; i < cmpnts.length; i++)
        {
            Component c = cmpnts[i];
            
            // --- EVIL HACK ---
            setSize(c.getSize());
                    
            PDFGraphics2D graphics = new PDFGraphics2D(file, c);
            graphics.startExport();
            c.print(graphics);
            graphics.endExport();
        }
    }
}//end class PDFExportFileType
