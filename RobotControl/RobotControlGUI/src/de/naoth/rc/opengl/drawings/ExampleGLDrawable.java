/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
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
    public Shader getShader(GL3 gl) {
        Shader shader = new Shader(gl, super.pathToGLSL, "vertex_shader.glsl", "texture_FS.glsl");
        shader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        shader.setGlobalUniform("light.intensities", new float[]{5f, 5f, 5f});
        return shader;
    }
    
    @Override
    public Texture getTexture() {
        return new Texture(super.pathToRER, "replacement.png");
    }
    
    @Override
    public GLData getGLData() {
        return new ObjFile(pathToRER, "ball.obj").buildGLData();
    }
    
}