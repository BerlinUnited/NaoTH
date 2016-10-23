/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.model.GLComplex;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.representations.GLCache;
import java.awt.Color;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 *
 * @author robert
 */
public class Nao implements GLComplex {    
    /*
    protected Color color;
    protected Color leftEyeLightColor;
    protected Color rightEyeLightColor;
    */
    protected final GL3 gl;
    
    protected GLCache glCache;
    
    protected ConcurrentLinkedQueue<GLObject> displayQueue;

    public Nao(GL3 gl, GLCache glCache, ConcurrentLinkedQueue<GLObject> displayQueue) {
        this.gl = gl;

        this.displayQueue = displayQueue;
        this.glCache = glCache;        
    }
    
    @Override
    public void update() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
}
