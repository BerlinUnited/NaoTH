/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.drawings.Drawable;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.InputStream;
import simgl.drawings.ComplexDrawing;
import simgl.file.GLData;
import simgl.file.PlyFile;
import simgl.file.Texture;
import simgl.representations.Matrix4;
import simgl.representations.Primitive;

/**
 *
 * @author robert
 */
public class Field extends ComplexDrawing {

    int xLength = 9000;
    int yLength = 6000;
    int boundary = 700;

    int xLengthHalf = xLength / 2 + boundary;
    int yLengthHalf = yLength / 2 + boundary;    
    
    public Field(Object[] args) {
        super(1);
                
        //Draw texture        
        Drawable field2D = new de.naoth.rc.drawings.FieldDrawingSPL2013();
        BufferedImage image = new BufferedImage(xLengthHalf, yLengthHalf, BufferedImage.TYPE_BYTE_INDEXED);
        Graphics2D g2d = image.createGraphics();
        field2D.draw(g2d);
        g2d.dispose();
        
        Texture texture = new Texture(image);
        
        //mesh
        InputStream mesh_file = ClassLoader.getSystemResourceAsStream("de/naoth/rc/opengl/res/soccer_field.ply");
        GLData mesh = new PlyFile(mesh_file, "soccer_field.ply").buildGLData();
        
        //Offset        
        Matrix4 modelMatrix = new Matrix4();
        modelMatrix.scale((xLengthHalf*2)/1000, 0f, (yLengthHalf*2)/1000);

        
        super.primitives[0] = new Primitive("field".hashCode(), mesh, "TexturedParticleShader", texture);
        super.primitives[0].putProperty("modelMatrix", modelMatrix);
    }  
    
}
