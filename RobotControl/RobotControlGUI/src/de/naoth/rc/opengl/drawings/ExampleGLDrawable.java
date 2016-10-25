/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Primitive;


/**
 *
 * @author Robert
 */
public class ExampleGLDrawable extends GLDrawable{
    
    public ExampleGLDrawable(String args[]) {
        super("example", 1);
        
        GLData mesh;
        Matrix4 offset;
        
         mesh = new ObjFile(super.pathToRES, "ball.obj").buildGLData();
        
        //Offset
        offset = new Matrix4();
        offset.translate(((float)Math.random() * 9)-4.5f, 0.2f, ((float)Math.random() * 6)-3f);
        offset.scale(0.2f, 0.2f, 0.2f);
        
        super.primitives[0] = new Primitive("head", mesh, "ColorShader", offset, new Texture(super.pathToRES, "replacement.png"));
    }
    
}