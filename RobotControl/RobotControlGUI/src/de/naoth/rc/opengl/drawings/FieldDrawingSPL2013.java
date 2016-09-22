/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.drawings.Drawable;
import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.representations.Point3f;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.image.BufferedImage;

/**
 *
 * @author robert
 */
public class FieldDrawingSPL2013 extends GLDrawable {

    int xLength = 9000;
    int yLength = 6000;
    int boundary = 700;

    int xLengthHalf = xLength / 2 + boundary;
    int yLengthHalf = yLength / 2 + boundary;

    @Override
    public Texture getTexture() {
        Drawable field2D = new de.naoth.rc.drawings.FieldDrawingSPL2013();

        BufferedImage image = new BufferedImage(xLengthHalf, yLengthHalf, BufferedImage.TYPE_BYTE_INDEXED);
        Graphics2D g2d = image.createGraphics();

        field2D.draw(g2d);

        g2d.dispose();
        return new Texture(image);
    }

    @Override
    public GLData getGLData() {
        return new ObjFile(pathToRES, "soccer_field.obj").buildGLData();
    }

    @Override
    public Point3f getScale() {
        return new Point3f((xLengthHalf*2)/1000, 0f, (yLengthHalf*2)/1000);
    }
}
