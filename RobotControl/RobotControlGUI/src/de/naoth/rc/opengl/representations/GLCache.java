/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.representations;

import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.model.GLObject;
import java.util.concurrent.ConcurrentHashMap;

/**
 *
 * @author robert
 */
public class GLCache {
    
    protected ConcurrentHashMap<String, GLObject> cachedObjects;
    protected ConcurrentHashMap<String, Boolean> objectsToBeCreated;
    
    protected ConcurrentHashMap<String, Shader> shader;
    
    public GLCache() {
        this.cachedObjects = new ConcurrentHashMap();
        this.objectsToBeCreated = new ConcurrentHashMap();
        
        this.shader = new ConcurrentHashMap();
    }
    
    public void touch(String key) {
        this.objectsToBeCreated.put(key, true);
    }

    public void put(String key, GLObject gLObject) {
        this.objectsToBeCreated.put(key, true);
        this.cachedObjects.put(key, gLObject);
    }
    
    public void putShader(String key, Shader value) {
        this.shader.put(key, value);
    }
    
    public GLObject get(String key) {
        return this.cachedObjects.get(key);
    }
    
    public Shader getShader(String key) {
        return this.shader.get(key);
    }
    
    public boolean isTouched(String key) {
        return this.objectsToBeCreated.containsKey(key);
    }
    
    public boolean containsKey(String key) {
        return this.cachedObjects.containsKey(key);
    }
    
    public boolean containsShaderKey(String key) {
        return this.shader.containsKey(key);
    }
}
