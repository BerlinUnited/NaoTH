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
import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Primitive;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

/**
 *
 * @author robert
 */
public class Field extends GLDrawable2 {

    int xLength = 9000;
    int yLength = 6000;
    int boundary = 700;

    int xLengthHalf = xLength / 2 + boundary;
    int yLengthHalf = yLength / 2 + boundary;    
    
    public Field(String[] args) {
        
        super.primitives = new Primitive[1];
        
        //Draw texture        
        Drawable field2D = new de.naoth.rc.drawings.FieldDrawingSPL2013();

        BufferedImage image = new BufferedImage(xLengthHalf, yLengthHalf, BufferedImage.TYPE_BYTE_INDEXED);
        Graphics2D g2d = image.createGraphics();

        field2D.draw(g2d);

        g2d.dispose();
        
        Texture texture = new Texture(image);
        
        //mesh
        GLData mesh = new ObjFile(pathToRES, "soccer_field.obj").buildGLData();
        
        //Offset
        Matrix4 offset = new Matrix4();
        offset.scale((xLengthHalf*2)/1000, 0f, (yLengthHalf*2)/1000);
        
        super.primitives[0] = new Primitive("field", mesh, "TexturedModelShader", offset, texture);
    }  
    
}
