/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.file.Texture;


/**
 *
 * @author Robert
 */
public class ExampleGLDrawable extends GLDrawable{
    
    public ExampleGLDrawable() {

    }
    
    @Override
    public Texture getTexture() {
        return new Texture(super.pathToRES, "replacement.png");
    }
    
    @Override
    public GLData getGLData() {
        return new ObjFile(pathToRES, "ball.obj").buildGLData();
    }
    
}