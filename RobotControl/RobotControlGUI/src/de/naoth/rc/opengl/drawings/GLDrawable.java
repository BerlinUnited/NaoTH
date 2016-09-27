/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.representations.Point3f;

/**
 *
 * @author robert
 */
public abstract class GLDrawable {
    
    public String pathToRES = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/res/";    
    
    public String getName() {
        return String.valueOf(System.currentTimeMillis());
    }
    
    public Shader getShader(GL3 gl) {
        return null;
    }
    
    public Texture getTexture() {
        return null;
    }
    
    public float[] getColor() {
        return new float[]{0f, 0f, 0f, 0f};
    }
    
    public GLData getGLData() {
        return null;
    }
    
    public Point3f getScale() {
        return new Point3f(1f,1f,1f);
    }
    
    public Point3f getPos() {
        return new Point3f();
    }
}