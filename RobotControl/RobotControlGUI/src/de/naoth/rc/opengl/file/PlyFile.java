/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.file;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;

/**
 *
 * @author robert
 */
public class PlyFile implements GLFile {
    
    private FloatBuffer coordinates;
    private IntBuffer indices;
   
    private String name, path;
    
    public PlyFile(String path, String fileName) {
    	this.path = path;

        this.name = fileName.split("\\.")[0];
    }

    @Override
    public GLData buildGLData() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public String getName() {
        return this.name;
    }
    
}
