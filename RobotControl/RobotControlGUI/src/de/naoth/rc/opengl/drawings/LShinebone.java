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

/**
 *
 * @author robert
 */
public class LShinebone extends GLDrawable {
    
    @Override
    public Shader getShader(GL3 gl) {
        Shader shader = new Shader(gl, super.pathToGLSL, "color_VS.glsl", "color_FS.glsl");
        shader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        shader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});
        return shader;
    }
        
    @Override
    public GLData getGLData() {
        return new ObjFile(pathToRES, "LShinebone.obj").buildGLData();
    }
    
    @Override
    public String getName() {
        return "LShinebone";
    }
}
