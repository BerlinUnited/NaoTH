package de.naoth.rc.opengl.file;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import de.naoth.rc.opengl.representations.Point2f;
import de.naoth.rc.opengl.representations.Point3f;

/**
 * This .obj Loader parses only vertex, normal, texture and index attributes,
 * it doesn't support materials or multiple objects in one file.
 * @author robert
 */
public class ObjFile implements GLFile {
    protected String objName;
    
    protected ArrayList<Integer> glIndexData;

    protected ArrayList<Point3f> glVertexData;
    protected ArrayList<Point3f> normalData;
    protected ArrayList<Point2f> textureData;
    
    protected ArrayList<Point3f> glNormalData;
    protected ArrayList<Point2f> glTextureData;
    
    protected boolean hasNormal = false;
    protected boolean hasTexture = false;
    
    protected String path;
    protected String objFileName;

    public ObjFile(String path, String objFileName) {
    	this.path = path;
    	
    	glIndexData = new ArrayList<>();
    	
        glVertexData = new ArrayList<>();
        normalData = new ArrayList<>();
        textureData = new ArrayList<>();

        this.objFileName = objFileName;
    }

    private void parseObjFile(String objFilename) {
    	boolean ones = true;
    	
    	BufferedReader bufferedReader = null;
    	try {
    		bufferedReader = new BufferedReader(new FileReader(new File(objFilename)));
    	}
    	catch(IOException io){
    		System.out.println("Can't open: " + objFilename);
    	}
    	
        String line = null;
        String[] values;
        while (true) {
        	
        	try {
        		 line = bufferedReader.readLine();
        	}
        	catch(IOException io){
        		System.out.println("Can't read: " + objFilename);
        	}
        	
            if (null == line) {
                break;
            }
            if (line.length() == 0 || line.startsWith("#")) {
                continue;
            }
            if (line.startsWith("vn")) {
                hasNormal = true;
                values = line.split("\\s+");
                normalData.add(new Point3f(Float.parseFloat(values[1]), Float.parseFloat(values[2]), Float.parseFloat(values[3])));
            } else if (line.startsWith("vt")) {
                hasTexture = true;
                values = line.split("\\s+");
                textureData.add(new Point2f(Float.parseFloat(values[1]), Float.parseFloat(values[2])));
            } else if (line.startsWith("v")) {
                values = line.split("\\s+");
                glVertexData.add(new Point3f(Float.parseFloat(values[1]), Float.parseFloat(values[2]), Float.parseFloat(values[3])));
            } else if (line.startsWith("f")) {
            	if(ones){ // Do this only ones
                	glNormalData = new ArrayList<>(glVertexData.size());
                	glTextureData = new ArrayList<>(glVertexData.size());
                	ones = false;
            	}
                processFace(line);
            } else if (line.startsWith("o")) {
                values = line.split("\\s+");
                objName = values[1];
            }
        }
        
    	try {
    		bufferedReader.close();
    	}
    	catch(IOException io){
    		System.out.println("Can't close: " + objFilename);
   		}
    }

    private void processFace(String line) {    	
    	while(glNormalData.size() < glVertexData.size()) glNormalData.add(null);
    	while(glTextureData.size() < glVertexData.size()) glTextureData.add(null);
    	
        String[] cornerData = line.split("\\s+");
        String[] indices;
        
        int faceIndex, textureIndex, normalIndex;
        Point3f vertex = new Point3f();
        Point3f normal = new Point3f();
        Point2f texture = new Point2f();
        
        
        for (int i = 1; i < cornerData.length; i++) {
            indices = cornerData[i].split("/");
            
            faceIndex = Integer.parseInt(indices[0]) - 1;
            vertex = glVertexData.get(faceIndex);
            
            if(hasTexture){
            	textureIndex = Integer.parseInt(indices[1]) - 1;
                texture = textureData.get(textureIndex);
            } else { // generate texture coordinates
            	/*
                if(i%3 == 0) texture = new Point2f(0.0f, 0.0f);
                else if(i%3 == 1) texture = new Point2f(1.0f, 1.0f);
                else texture = new Point2f(1.0f, 0.0f);
                */
            }
           
            if(hasNormal) {
            	normalIndex = Integer.parseInt(indices[2]) - 1;
                normal = normalData.get(normalIndex);
            } else { // generate normal ;)
                normal = vertex;
            }
            
            if(glNormalData.get(faceIndex) == null) {
            	if(hasTexture) glTextureData.set(faceIndex, texture);
            	glNormalData.set(faceIndex, normal);
            	
            	glIndexData.add(faceIndex);
            }
            else if (glNormalData.get(faceIndex) == normal && (!hasTexture || glTextureData.get(faceIndex) == texture)) {
            	glIndexData.add(faceIndex);
            }
            else {
            	glVertexData.add(vertex);
            	if(hasTexture) glTextureData.add(texture);
            	glNormalData.add(normal);
            	
            	glIndexData.add(glVertexData.size()-1);
            }          
            
        }
    }

    @Override
    public String getName() { 
    	return objName;
    }

    @Override
    public GLData buildGLData() {
    	parseObjFile(this.path + this.objFileName);
    	if(hasTexture) {
    		return new GLData(glVertexData, glNormalData, glTextureData, glIndexData);
    	} else {
    		return new GLData(glVertexData, glNormalData, glIndexData);
    	}
        
    }
}
