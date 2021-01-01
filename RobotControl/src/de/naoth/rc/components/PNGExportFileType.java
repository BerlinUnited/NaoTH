/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components;

import java.awt.Component;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Properties;
import javax.imageio.ImageIO;
import org.freehep.util.export.ExportFileType;

/**
 *
 * @author Heinrich Mellmann
 */
public class PNGExportFileType extends ExportFileType
{
    @Override
    public String getDescription() {
        return "Portable Network Grapgic";
    }

    @Override
    public String[] getExtensions() {
        return new String[]{"png"};
    }

    @Override
    public String[] getMIMETypes() {
        return new String[]{"image/png"};
    }

    @Override
    public void exportToFile(OutputStream out, Component[] cmpnts, Component exportDlg, Properties prprts, String creator) throws IOException {
        for(int i = 0; i < cmpnts.length; i++)
        {
            Component c = cmpnts[i];

            BufferedImage bi = new BufferedImage(c.getWidth(), c.getHeight(), BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2d = bi.createGraphics();

            c.paintAll(g2d);


            ImageIO.write(bi, "PNG", out);
        }
    }

    @Override
    public void exportToFile(File file, Component[] cmpnts, Component exportDlg, Properties prprts, String string) throws IOException {
        for(int i = 0; i < cmpnts.length; i++)
        {
            Component c = cmpnts[i];

            BufferedImage bi = new BufferedImage(c.getWidth(), c.getHeight(), BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2d = bi.createGraphics();

            c.paintAll(g2d);


            ImageIO.write(bi, "PNG", file);
        }
    }    
}//end PNGExportFileType
